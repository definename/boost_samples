#include "pch.h"

//! Acceptor pointer type.
using AcceptorPtr = boost::shared_ptr<boost::asio::ip::tcp::acceptor>;
//! SSL context type.
using SSLContext = boost::asio::ssl::context;
//! SSL context pointer type.
using SSLContextPtr = boost::shared_ptr<SSLContext>;
//! SSL socket type.
using SSLSocket = boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;
//! Socket pointer type.
using SSLSocketPtr = boost::shared_ptr<SSLSocket>;

//! Accept.
void Accept(AcceptorPtr acc, SSLContextPtr ctx);
//! Handles read.
void HandleRead(SSLSocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Handles write.
void HandleWrite(SSLSocketPtr socket, const boost::system::error_code& ec, std::size_t transferred);
//! Read buffer.
char buf[512];

int main()
{
	try
	{
		SSLContextPtr ctx = boost::make_shared<SSLContext>(boost::asio::ssl::context::sslv23);
		ctx->set_options(SSLContext::default_workarounds | SSLContext::no_sslv2);
		ctx->use_rsa_private_key_file("c://server.key", SSLContext::pem);
		ctx->use_certificate_file("c://server.crt", SSLContext::pem);

		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), 2001);
		boost::asio::io_service io;
		Accept(boost::make_shared<boost::asio::ip::tcp::acceptor>(io, ep), ctx);
		std::cout << "Server has been started on: " << ep.port() << std::endl;
		io.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}

void Accept(AcceptorPtr ac, SSLContextPtr ctx)
{
	SSLSocketPtr socket = boost::make_shared<SSLSocket>(ac->get_io_service(), *ctx);
	ac->async_accept(socket->lowest_layer(), [socket, ac, ctx](const boost::system::error_code& ec)
	{
		if (ec)
		{
			std::cerr << "Acceptance error: " << ec.message() << std::endl;
		}
		else
		{
			boost::asio::ip::tcp::endpoint ep = socket->lowest_layer().remote_endpoint();
			std::cout << "Accepted: " << ep.address().to_string() << ":" << ep.port() << std::endl;

			socket->async_handshake(SSLSocket::server, [socket](const boost::system::error_code& ec)
			{
				if (ec)
				{
					std::cerr << "Handshake error: " << ec.message() << std::endl;
					return;
				}

				HandleRead(socket, boost::system::error_code(), 0);
			});
		}

		Accept(ac, ctx);
	});
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