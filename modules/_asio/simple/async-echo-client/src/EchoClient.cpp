#include "pch.h"
#include "EchoClient.h"

EchoClient::EchoClient(const std::string& msg, boost::asio::io_service& io)
	: msg_(msg)
	, socket_(io)
	, isStarted_(true)
{ }

EchoClient::~EchoClient()
{
	try {
		Stop();
	}
	catch (const std::exception& e) {
		std::cerr << "Echo client destruction error: " << e.what() << std::endl;
	}
}

void EchoClient::Connect(const boost::asio::ip::tcp::endpoint& ep)
{
	socket_.async_connect(ep, MEM_FN2(OnConnect, ep, _1));
}

EchoClient::Ptr EchoClient::Start(
	const boost::asio::ip::tcp::endpoint& ep,
	const std::string& msg,
	boost::asio::io_service& io)
{
	EchoClient::Ptr client(new EchoClient(msg, io));
	client->Connect(ep);
	return client;
}

void EchoClient::Stop()
{
	if (!IsStarted()) {
		std::cerr << "Client has already stopped" << std::endl;
		return;
	}

	std::cout << "Echo client is being stopped" << std::endl;
	isStarted_ = false;

	boost::system::error_code ec;
	socket_.shutdown(boost::asio::socket_base::shutdown_both, ec);
	socket_.close(ec);
}

bool EchoClient::IsStarted() const
{
	return isStarted_;
}

void EchoClient::OnConnect(
	const boost::asio::ip::tcp::endpoint& ep,
	const boost::system::error_code& ec)
{
	if (ec) {
		std::cerr << "Unable to connect to the endpoint: " << ep << std::endl;
		Stop();
	}
	else {
		std::cout << "Successfully connected to the endpoint: " << ep << std::endl;

		std::copy(msg_.begin(), msg_.end(), wrtBuff_);
		boost::asio::async_write(
			socket_,
			boost::asio::buffer(wrtBuff_, msg_.size()),
			MEM_FN2(OnWrite, _1, _2));
	}
}

std::size_t EchoClient::OnCompletion(const boost::system::error_code& ec, std::size_t bytes)
{
	if (ec) {
		std::cerr << "Completion error: " << ec.message() << std::endl;
		return 0;
	}
	bool found = std::find(readBuff_, readBuff_ + bytes, '\0') < readBuff_ + bytes;
	return found ? 0 : 1;
}

void EchoClient::OnRead(const boost::system::error_code& ec, std::size_t bytes)
{
	if (ec) {
		std::cerr << "Unable to read from the socket: " << ec.message() << std::endl;
		return;
	}
	std::cout << ":: Server echoed with: " << std::string(readBuff_, bytes) << std::endl;
}

void EchoClient::OnWrite(const boost::system::error_code& ec, std::size_t bytes)
{
	if (ec) {
		std::cerr << "Unable to send message: " << ec.message() << std::endl;
		return;
	}
	boost::asio::async_read(
		socket_,
		boost::asio::buffer(readBuff_),
		MEM_FN2(OnCompletion, _1, _2),
		MEM_FN2(OnRead, _1, _2));
}