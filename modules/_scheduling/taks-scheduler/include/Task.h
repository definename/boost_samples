#pragma once

template <typename T>
class Task
{
	//
	// Public types.
	//
public:
	//! Task status
	enum class Status
	{
		QueuedUp,
		Pending,
		Stopped
	};
	//! Task duration type.
	using Duration = std::chrono::steady_clock::duration;
	//! Task future type.
	using Future = std::shared_future<void>;
	//! Task promise type.
	using Promise = std::promise<Status>;

	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Task(
		T const& type,
		Duration const& delay,
		std::function<void(void)> const& callback,
		bool const& repeat);
	//! Destructor.
	~Task();

	//
	// Public interface.
	//
public:
	//! Function call operator overloading.
	void operator()() const;
	//! Returns task delay.
	Duration const& GetDelay() const;
	//! Returns reference to the task future object.
	Future& GetFuture();
	//! Returns reference to the task promise object.
	Promise& GetPromise();
	//! Returns reference to the task repeatable flag.
	std::atomic<bool> const& GetRepeat() const;
	//! Returns reference to the task status.
	std::atomic<Status>& GetStatus();
	//! Returns task type.
	T const& GetType() const;

	//
	// Private data members.
	//
private:
	//! Task callback.
	std::function<void(void)> callback_;
	//! Task delay.
	Duration delay_;
	//! Callback result.
	Future future_;
	//! Promise value.
	Promise promise_;
	//! Task repeatable flag.
	std::atomic<bool> repeat_;
	//! Task status.
	std::atomic<Status> status_;
	//! Task type
	T type_;
};

template<typename T>
Task<T>::Task(
	T const& type,
	typename Task<T>::Duration const& delay,
	std::function<void(void)> const& callback,
	bool const& repeat)
	: callback_(callback)
	, delay_(delay)
	, repeat_(repeat)
	, status_(Status::QueuedUp)
	, type_(type)
{ }

template<typename T>
Task<T>::~Task()
{ }

template<typename T>
void Task<T>::operator()() const
{
	callback_();
}

template<typename T>
typename Task<T>::Duration const& Task<T>::GetDelay() const
{
	return delay_;
}

template<typename T>
typename Task<T>::Future& Task<T>::GetFuture()
{
	return future_;
}

template<typename T>
typename Task<T>::Promise& Task<T>::GetPromise()
{
	return promise_;
}

template<typename T>
std::atomic<bool> const& Task<T>::GetRepeat() const
{
	return repeat_;
}

template<typename T>
std::atomic<typename Task<T>::Status>& Task<T>::GetStatus()
{
	return status_;
}

template<typename T>
T const& Task<T>::GetType() const
{
	return type_;
}