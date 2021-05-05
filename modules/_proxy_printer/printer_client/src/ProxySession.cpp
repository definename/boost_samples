#include "pch.h"
#include "ProxySession.h"

ProxySession::ProxySession(boost::asio::io_service& ioService)
	: socket_(ioService)
{ }

ProxySession::~ProxySession()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Session destruction error: " << e.what() << std::endl;
	}
}

ProxySession::Socket& ProxySession::GetSocket()
{
	return socket_;
}

size_t ProxySession::GetTransferred()
{
	return transferred_;
}

char* ProxySession::GetBuffer()
{
	return buffer_;
}

void ProxySession::Stop()
{
	if (socket_.is_open())
		socket_.close();
}

void ProxySession::SetTransferred(const size_t& transferred)
{
	transferred_ = transferred;
}