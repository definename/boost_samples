#include "pch.h"
#include "IPCClient.h"

namespace ipc
{

IPCClient::IPCClient(const std::string& name)
	: name_(name)
	, timer_(new boost::asio::deadline_timer(*ioService_))
{
	Run();

	timer_->expires_at(boost::posix_time::pos_infin);
	timer_->async_wait(boost::bind(
		&IPCClient::HandleTimeout, this, boost::asio::placeholders::error));
}

IPCClient::~IPCClient()
{
	try
	{
		if (!boost::interprocess::message_queue::remove(name_.c_str()))
			std::cout << "Unable to remove queue: " << name_;

		boost::system::error_code ec;
		timer_->cancel(ec);
		Stop();
	}
	catch (const std::exception& e)
	{
		std::cerr << "IPC client destruction error: " << e.what() << std::endl;
	}
}

void IPCClient::Create()
{
	try
	{
// 		if (!boost::interprocess::message_queue::remove(name_.c_str()))
// 			std::cout << "Unable to remove queue: " << name_ << std::endl;

		mq_.reset(new boost::interprocess::message_queue(
			boost::interprocess::open_or_create,
			name_.c_str(),
			MSG_NUM,
			MSG_SIZE));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unable to create message queue: " << e.what() << std::endl;
	}
}

boost::signals2::connection IPCClient::DoOnReceive(const OnReceive::slot_type& signal)
{
	return onReceive_.connect(signal);
}

void IPCClient::Open()
{
	try
	{
		if (IsRunning())
		{
			mq_.reset(new boost::interprocess::message_queue(
				boost::interprocess::open_only,
				name_.c_str()));

			ioService_->post(boost::bind(&IPCClient::DoReceive, this));
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unable to open message queue: " << e.what() << std::endl;

		timer_->expires_from_now(boost::posix_time::seconds(2));
		timer_->async_wait(boost::bind(
			&IPCClient::HandleTimeout, this, boost::asio::placeholders::error));
	}
}

void IPCClient::DoReceive()
{
	try
	{
		while (IsRunning())
		{
			std::string data(MSG_SIZE, '0');
			boost::interprocess::message_queue::size_type size;
			unsigned int priority;
			if (mq_->try_receive(&data[0], data.size(), size, priority))
			{
				data.resize(size);
				onReceive_(data);
			}
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Receiver error: " << e.what() << std::endl;
		Open();
	}
}

void IPCClient::HandleTimeout(const boost::system::error_code& ec)
{
	try
	{
		if (ec && ec.value() != boost::asio::error::operation_aborted)
			BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

		if (timer_->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			Open();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Timeout handling error: " << e.what() << std::endl;
	}
}

void IPCClient::Send(const std::string& data)
{
	try
	{
		if (!mq_)
			BOOST_THROW_EXCEPTION(std::runtime_error("Invalid comunication pointer"));

		mq_->try_send(data.data(), data.size(), 1);
	}
	catch (const std::exception& e)
	{
		std::cerr << "Unable to send message: " << e.what() << std::endl;
	}
}

}