#include "pch.h"
#include "nat/Session.h"
#include "nat/Message.h"

namespace opennat
{

Session::Session(boost::asio::io_service& ioService)
	: deadline_(new boost::asio::deadline_timer(ioService))
	, id_(boost::uuids::nil_uuid())
	, isReady_(false)
	, remoteId_(boost::uuids::nil_uuid())
	, socket_(new boost::asio::ip::tcp::socket(ioService))
	, type_(types::UNKNOWN)
	, timer_(new boost::asio::deadline_timer(ioService))
	, timeout_(0)
{ }

Session::~Session()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "Session destruction error: " << e.what();
	}
}

types::SessionPtr Session::Construct(boost::asio::io_service& ioService)
{
	boost::shared_ptr<Session> session(new Session(ioService));
	return session;
}

types::TimerPtr Session::GetDeadline()
{
	return deadline_;
}

boost::uuids::uuid Session::GetRemoteId() const
{
	return remoteId_;
}

types::StreamBuff& Session::GetStream()
{
	return buffer_;
}

boost::uuids::uuid Session::GetId() const
{
	return id_;
}

types::SocketPtr Session::GetSocket() const
{
	return socket_;
}

unsigned int& Session::GetTimeout()
{
	return timeout_;
}

types::TimerPtr Session::GetTimer()
{
	return timer_;
}

types::SessionType Session::GetType() const
{
	return type_;
}

bool Session::IsReady() const
{
	return isReady_;
}

void Session::SetId(const boost::uuids::uuid& id)
{
	id_ = id;
}

void Session::SetRemoteId(const boost::uuids::uuid& id)
{
	remoteId_ = id;
}

void Session::SetReady(const bool ready)
{
	isReady_ = ready;
}

void Session::SetType(const types::SessionType type)
{
	type_ = type;
}

void Session::Stop()
{
	if (socket_ && socket_->is_open())
	{
		boost::system::error_code ec;
		socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		socket_->close(ec);
		deadline_->cancel(ec);
		timer_->cancel(ec);
		LDBG_ << "Session: " << boost::uuids::to_string(id_) << " has been stopped";
	}
}

void Session::SetBytesTransferred(const std::size_t bytes)
{
	bytes_ = bytes;
}

std::size_t Session::GetBytesTransferred() const
{
	return bytes_;
}

}