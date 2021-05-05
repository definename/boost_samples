#include "pch.h"
#include "proxy/Core.h"

namespace proxy
{

Core::Core()
	: ioService_(new boost::asio::io_service)
	, ioWork_(new boost::asio::io_service::work(*ioService_))
	, ioStrand_(new boost::asio::io_service::strand(*ioService_))
	, isRunning_(false)
	, workers_{2}
{ }

Core::Core(const unsigned int workers)
	: ioService_(new boost::asio::io_service)
	, ioWork_(new boost::asio::io_service::work(*ioService_))
	, ioStrand_(new boost::asio::io_service::strand(*ioService_))
	, isRunning_(false)
	, workers_{workers}
{ }

Core::~Core()
{ }

bool Core::IsRunning() const
{
	return isRunning_;
}

void Core::Run()
{
	if (!isRunning_)
	{
		for (unsigned int i = 0; i != workers_; ++i)
		{
			threads_.emplace_back(boost::bind(&boost::asio::io_service::run, ioService_));
		}
		isRunning_ = true;
	}
}

void Core::Stop()
{
	if (isRunning_)
	{
		isRunning_ = false;
		if (ioWork_)
			ioWork_.reset();
		if (ioService_)
			ioService_->stop();
		for (auto& val : threads_)
		{
			val.join();
		}
		threads_.clear();
	}
}

}