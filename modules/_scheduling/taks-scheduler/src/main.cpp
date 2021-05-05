#include "pch.h"
#include "App.h"
#include "Log.h"

#include "Scheduler.h"

enum class TaskType
{
	Ping,
	Pong,
	Report
};

using TaskPtr = std::shared_ptr<Task<TaskType>>;
using SchedulerPtr = std::shared_ptr<Scheduler<TaskType>>;

int main()
{
	try
	{
		auto callback1 = []() { LDBG_ << "Task=1= has been scheduled"; };
		auto callback2 = []() { LDBG_ << "Task>2< has been scheduled"; };
		auto callback3 = []() { LDBG_ << "Task<3> has been scheduled"; };

		TaskPtr task1(new Task<TaskType>(
			TaskType::Ping,
			std::chrono::seconds(1),
			callback1,
			true));

		TaskPtr task2(new Task<TaskType>(
			TaskType::Pong,
			std::chrono::seconds(5),
			callback2,
			true));

		TaskPtr task3(new Task<TaskType>(
			TaskType::Report,
			std::chrono::seconds(10),
			callback3,
			false));

		SchedulerPtr sc = std::make_shared<Scheduler<TaskType>>();
		sc->Schedule(task1);
		sc->Schedule(task2);
		sc->Schedule(task3);

		std::this_thread::sleep_for(std::chrono::seconds(11));
		sc->StopTask(TaskType::Pong);

		App app;
		app.WaitForTerminationRequest();
	}
	catch (std::exception const& e)
	{
		LERR_ << "Error occurred: " << e.what();
	}

	return 0;
}