#include "pch.h"
#include "ProxySession.h"

ProxySession::ProxySession(boost::asio::io_service& ioService)
	: socket_(ioService)
	, isConnected_(false)
	, isConnecting_(false)
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

bool ProxySession::IsConnected()
{
	return isConnected_;
}

bool ProxySession::IsConnecting()
{
	return isConnecting_;
}

void ProxySession::Stop()
{
	if (socket_.is_open())
	{
		while (!inputQueue_.empty())
			inputQueue_.pop();

		isConnecting_ = false;
		isConnected_ = false;
		socket_.close();
	}
}

void ProxySession::SetConnected(bool isConnected)
{
	isConnected_ = isConnected;
}

void ProxySession::SetConnecting(bool isConnecting)
{
	isConnecting_ = isConnecting;
}

void ProxySession::SetTransferred(const size_t& transferred)
{
	transferred_ = transferred;
}