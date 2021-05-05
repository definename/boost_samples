#pragma once

#include "Task.h"

template<typename T>
class Scheduler : public std::enable_shared_from_this<Scheduler<T>>
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Scheduler();
	//! Destructor.
	~Scheduler();

	//
	// Public types.
	//
public:
	//! Is scheduler running.
	bool IsRunning() const;
	//! Schedules given task.
	void Schedule(std::shared_ptr<Task<T>> task);
	//! Stops scheduler.
	void Stop();
	//! Stops task with given type.
	void StopTask(T const& type);

	//
	// Private types.
	//
private:
	//! Task list type.
	using TaskList = std::map<T, std::shared_ptr<Task<T>>>;

	//
	// Private interface
	//
private:
	//! Runs scheduler.
	void Run();

	//
	// Private data members.
	//
private:
	//! Condition variable.
	std::condition_variable_any cv_;
	//! Synchronization mutex.
	std::mutex mutex_;
	//! Is scheduler running flag.
	std::atomic<bool> isRunning_;
	//! Task list.
	TaskList taskList_;
	//! Scheduler watcher.
	std::shared_future<void> watcher_;
};

template<typename T>
Scheduler<T>::Scheduler()
	: isRunning_(true)
{
	watcher_ = std::async(
		std::launch::async, std::bind(&Scheduler<T>::Run, this));
}

template<typename T>
Scheduler<T>::~Scheduler()
{
	try
	{
		Stop();
	}
	catch (std::exception const& e)
	{
		LERR_ << "Task scheduler destruction error: " << e.what();
	}
}

template<typename T>
bool Scheduler<T>::IsRunning() const
{
	return isRunning_;
}

template<typename T>
void Scheduler<T>::Schedule(std::shared_ptr<Task<T>> task)
{
	{
		std::unique_lock<std::mutex> l(mutex_);
		if (!taskList_.emplace(task->GetType(), task).second)
			BOOST_THROW_EXCEPTION(std::runtime_error("Unable to schedule task with given type"));
	}
	cv_.notify_all();
}

template<typename T>
void Scheduler<T>::Run()
{
	while (IsRunning())
	{
		std::unique_lock<std::mutex> l(mutex_);
		for (auto it = taskList_.begin(); it != taskList_.end();)
		{
			auto task = (*it).second;
			if (task->GetStatus() == Task<T>::Status::QueuedUp)
			{
				if (task->GetFuture().valid())
					task->GetFuture().wait();

				task->GetStatus() = Task<T>::Status::Pending;
				auto self(shared_from_this());
				task->GetFuture() = std::async(std::launch::async, [task, self, this]()
				{
					try
					{
						std::future<Task<T>::Status> f(task->GetPromise().get_future());
						switch (f.wait_for(task->GetDelay()))
						{
						case std::future_status::timeout:
						{
							(*task)();

							task->GetStatus() =
								task->GetRepeat() ?
								Task<T>::Status::QueuedUp :
								Task<T>::Status::Stopped;

							break;
						}
						case std::future_status::ready:
						{
							task->GetStatus() = f.get();

							break;
						}
						default:
							BOOST_THROW_EXCEPTION(std::runtime_error("Invalid task status"));
						}
						Task<T>::Promise().swap(task->GetPromise());
					}
					catch (std::exception const& e)
					{
						LERR_ << "Scheduled task failed: " << e.what();
						task->GetStatus() = Task<T>::Status::Stopped;
					}
					cv_.notify_all();
				});
			}
			else if (task->GetStatus() == Task<T>::Status::Stopped)
			{
				it = taskList_.erase(it);
				continue;
			}

			++it;
		}

		while (IsRunning() &&
			!std::count_if(
				taskList_.begin(),
				taskList_.end(),
				[](TaskList::value_type const& val) {
			return val.second->GetStatus() != Task<T>::Status::Pending; }))
			cv_.wait(l);
	}
}

template<typename T>
void Scheduler<T>::Stop()
{
	if (!IsRunning())
	{
		LERR_ << "Scheduler is already stopped";
		return;
	}

	isRunning_ = false;
	{
		std::unique_lock<std::mutex> l(mutex_);
		for (auto val : taskList_)
		{
			auto task = val.second;
			if (task->GetStatus() != Task<T>::Status::Pending)
			{
				task->GetStatus() = Task<T>::Status::Stopped;
				continue;
			}

			task->GetPromise().set_value(Task<T>::Status::Stopped);
			task->GetFuture().wait();
		}
		taskList_.clear();
	}
	cv_.notify_all();

	if (watcher_.valid())
		watcher_.wait();

	LDBG_ << "Scheduler has been stopped";
}

template<typename T>
void Scheduler<T>::StopTask(T const& type)
{
	{
		std::unique_lock<std::mutex> l(mutex_);
		auto it = taskList_.find(type);
		if (it == taskList_.end())
		{
			LDBG_ << "There is no task with given type";
			return;
		}

		auto task = it->second;
		if (task->GetStatus() != Task<T>::Status::Pending)
		{
			task->GetStatus() = Task<T>::Status::Stopped;
		}
		else
		{
			task->GetPromise().set_value(Task<T>::Status::Stopped);
			task->GetFuture().wait();
		}
		taskList_.erase(it);
	}
	cv_.notify_all();
}