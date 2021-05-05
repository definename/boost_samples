#include "pch.h"

//! SSL context type.
using SSLContext = boost::asio::ssl::context;
//! SSL socket type.
using SSLSocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
//! Socket pointer type.
using SSLSocketPtr = boost::shared_ptr<SSLSocket>;

//! Handles read.
void HandleRead(SSLSocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Handles write.
void HandleWrite(SSLSocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Read buffer.
char buf[512];

int main()
{
	SSLContext ctx(boost::asio::ssl::context::sslv23);
	boost::asio::io_service io;
	SSLSocketPtr socket = boost::make_shared<SSLSocket>(io, ctx);
	socket->set_verify_mode(boost::asio::ssl::verify_none);

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 2001);
	socket->lowest_layer().async_connect(ep, [socket](const boost::system::error_code& ec)
	{
		if (ec)
		{
			std::cerr << "Connection error: " << ec.message() << std::endl;
			return;
		}

		socket->async_handshake(
			SSLSocket::client,
			[socket](const boost::system::error_code& ec)
		{
			if (ec)
			{
				std::cerr << "Handshake error: " << ec.message() << std::endl;
				return;
			}

			HandleRead(socket, boost::system::error_code(), 0);
			std::cout << "Connected" << std::endl;
		});
	});

	io.run();
	return 0;
}

void HandleRead(SSLSocketPtr socket, const boost::system::error_code& ec, std::size_t bytesRead)
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

void HandleWrite(SSLSocketPtr socket, const boost::system::error_code& ec, std::size_t bytesWrite)
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