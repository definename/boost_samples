#include "pch.h"

//! Socket pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
//! Handles read.
void HandleRead(SocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Handles write.
void HandleWrite(SocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Read buffer.
char buf[512];

int main()
{

	try
	{
		boost::system::error_code ec;
		std::string host("127.0.0.1");

		boost::asio::io_service io;
		boost::asio::ip::tcp::resolver resolver(io);

		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 2001);
		resolver.async_resolve(ep, [&io](
			const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it) {
			if (ec) {
				std::cout << "Unable to resolve given endpoint" << std::endl;
				return;
			}
			SocketPtr socket(new boost::asio::ip::tcp::socket(io));
			boost::asio::async_connect(*socket, it,
				[](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator next) {
				if (ec)
					std::cerr << "Unable to connect: " << ec.message() << std::endl;

				std::cout << "Trying to connect to: " << next->endpoint() << std::endl;
				return next;
			},
				[socket](const boost::system::error_code& ec, boost::asio::ip::tcp::resolver::iterator it) {
				if (ec) {
					std::cerr << "Unable to establish connection: " << ec.message() << std::endl;
					return;
				}
				std::cout << "Connected: " << it->endpoint() << std::endl;
				HandleRead(socket, boost::system::error_code(), 0);
			});
		});
		io.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
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

	char msg[] = "HELLO";
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