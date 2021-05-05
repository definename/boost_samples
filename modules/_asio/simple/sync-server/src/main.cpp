#include "pch.h"

typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

int main()
{
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::endpoint ep(
		boost::asio::ip::address::from_string("127.0.0.1"),
		2001);
	boost::asio::ip::tcp::acceptor acc(ioService, ep);

	while (true)
	{
		SocketPtr socket(new boost::asio::ip::tcp::socket(ioService));
		boost::asio::ip::tcp::endpoint remote;
		// Accept.
		acc.accept(*socket, remote);
		std::cout << "Accepted remote peer: " << remote.address().to_string() << ":" << remote.port() << std::endl;

		while (true)
		{
			// Read.
			char buf[512];
			boost::system::error_code ec;
			size_t len = socket->read_some(boost::asio::buffer(buf), ec);
			if (ec)
			{
				std::cerr << ec.message() << std::endl;
				break;
			}

			std::cout << std::string(buf, len) << std::endl;

			// Write.
			std::string msg("WELCOME");
			ec.clear();
			socket->write_some(boost::asio::buffer(msg), ec);
			if (ec)
			{
				std::cerr << ec.message() << std::endl;
				break;
			}
		}
	}

	return 0;
}