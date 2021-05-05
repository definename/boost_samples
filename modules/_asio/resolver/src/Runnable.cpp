#include "pch.h"
#include "Runnable.h"

Runnable::Runnable(void)
	: isRunning_(false)
{ }

Runnable::~Runnable(void)
{ }

bool Runnable::IsRunning(void) const
{
	return isRunning_;
}

void Runnable::Run(void)
{
	if (isRunning_)
	{
		return;
	}

	if (!work_)
		work_.reset(new boost::asio::io_service::work(ioService_));

	thread_ = boost::thread(boost::bind(&boost::asio::io_service::run, &ioService_));
	isRunning_ = true;
}

void Runnable::Stop(void)
{
	if (!IsRunning())
	{
		return;
	}

	isRunning_ = false;

	if (work_)
		work_.reset();

	ioService_.stop();
	thread_.join();
}
