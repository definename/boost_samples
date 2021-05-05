#include "pch.h"

//! Socket pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
//! Acceptor pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

//! Accept.
void Accept(AcceptorPtr acc);
//! Handles accept.
void HandleAccept(SocketPtr socket, AcceptorPtr acc, const boost::system::error_code& ec);
//! Handles read.
void HandleRead(SocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Handles write.
void HandleWrite(SocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Read buffer.
char buf[512];

int main()
{
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 2001);

	AcceptorPtr acc(new boost::asio::ip::tcp::acceptor(ioService, ep));
	Accept(acc);

	ioService.run();

	return 0;
}

void Accept(AcceptorPtr acc)
{
	SocketPtr socket(new boost::asio::ip::tcp::socket(acc->get_io_service()));
	acc->async_accept(*socket, boost::bind(HandleAccept, socket, acc, boost::asio::placeholders::error));
}

void HandleAccept(SocketPtr socket, AcceptorPtr acc, const boost::system::error_code& ec)
{
	if (ec)
	{
		std::cerr << ec.message() << std::endl;
		return;
	}

	boost::system::error_code er;
	boost::asio::ip::tcp::endpoint ep = socket->remote_endpoint(er);
	if (!er)
		std::cout << ep.address().to_string() << ":" << ep.port() << std::endl;

	HandleRead(socket, boost::system::error_code(), 0);
	Accept(acc);
}

void HandleRead(SocketPtr socket, const boost::system::error_code& ec, std::size_t bytesRead)
{
	if (ec)
	{
		if (ec == boost::asio::error::connection_reset)
			std::cout << "Connection reset by peer" << std::endl;
		else
			std::cerr << ec.message() << std::endl;

		return;
	}

	if (bytesRead > 0)
	{
		std::cout << "Bytes read: " << bytesRead << std::endl;
		std::cout << std::string(buf, bytesRead) << std::endl;
	}

	boost::asio::deadline_timer t(socket->get_io_service(), boost::posix_time::seconds(1));
	t.wait();
	//boost::this_thread::sleep_for(boost::chrono::seconds(1));

	char msg[] = "WELCOME";
	socket->async_write_some(
		boost::asio::buffer(msg),
		boost::bind(
		HandleWrite,
		socket,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}

void HandleWrite(SocketPtr socket, const boost::system::error_code& ec, std::size_t bytesWrite)
{
	if (ec)
	{
		if (ec == boost::asio::error::connection_reset)
			std::cout << "Connection reset by peer" << std::endl;
		else
			std::cerr << ec.message() << std::endl;

		return;
	}

	std::cout << "Bytes transferred: " << bytesWrite << std::endl;

	socket->async_read_some(
		boost::asio::buffer(buf),
		boost::bind(
		HandleRead,
		socket,
		boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
}