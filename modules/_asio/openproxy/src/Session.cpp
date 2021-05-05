#include "pch.h"
#include "proxy/Session.h"

namespace proxy
{

Session::Session(boost::asio::io_service& ioService)
	: isReady_(false)
	, socket_(new boost::asio::ip::tcp::socket(ioService))
	, type_(protocol::UNKNOWN)
	, clientId_(boost::uuids::nil_uuid())
	, id_(boost::uuids::nil_uuid())
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

types::Buff& Session::GetBuffRead()
{
	return buffRead_;
}

types::Buff& Session::GetBuffWrite()
{
	return buffWrite_;
}

size_t Session::GetBytesReadBuff() const
{
	return bytesRead_;
}

size_t Session::GetBytesWriteBuff() const
{
	return bytesWrite_;
}

boost::uuids::uuid Session::GetClientId() const
{
	return clientId_;
}

boost::uuids::uuid Session::GetId() const
{
	return id_;
}

bool Session::GetReady() const
{
	return isReady_;
}

types::SocketPtr Session::GetSocket() const
{
	return socket_;
}

protocol::StreamType Session::GetType() const
{
	return type_;
}

void Session::SetBytesReadBuff(const size_t size)
{
	bytesRead_ = size;
}

void Session::SetBytesWriteBuff(const size_t size)
{
	bytesWrite_ = size;
}

void Session::SetClientId(const boost::uuids::uuid& id)
{
	clientId_ = id;
}

void Session::SetId(const boost::uuids::uuid& id)
{
	id_ = id;
}

void Session::SetReady(const bool ready)
{
	isReady_ = ready;
}

void Session::SetType(const protocol::StreamType type)
{
	type_ = type;
}

void Session::Stop()
{
	if (socket_ && socket_->is_open())
	{
		isReady_ = false;
		boost::system::error_code ec;
		socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		socket_->close(ec);
		LDBG_ << "Session: " << boost::uuids::to_string(id_) << " has been stopped";
	}
}

}