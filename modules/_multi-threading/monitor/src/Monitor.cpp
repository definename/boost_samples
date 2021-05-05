#include "pch.h"
#include "Monitor.h"

Monitor::Monitor()
	: isRunning_(false)
{ }

Monitor::~Monitor()
{ }

void Monitor::StartMonitor()
{
	isRunning_ = true;
	while (true)
	{
		if (!isRunning_)
			break;

		std::cout << "Monitoring..." << std::endl;
		boost::this_thread::sleep_for(boost::chrono::milliseconds(200));
	}
	std::cout << "Monitor has been  stopped" << std::endl;
}

void Monitor::Stop()
{
	isRunning_ = false;
}

bool Monitor::IsRunning() const
{
	return isRunning_;
}