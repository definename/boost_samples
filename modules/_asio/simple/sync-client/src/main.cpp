#include "pch.h"

int main()
{
	boost::asio::io_service ioService;
	boost::asio::ip::tcp::socket socket(ioService);

	// Connect.
	boost::asio::ip::tcp::endpoint ep(
		boost::asio::ip::address::from_string("127.0.0.1"),
		2001);
	boost::system::error_code ec;
	socket.connect(ep, ec);
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		return EXIT_FAILURE;
	}

	// Write.
	std::string msg("HELLO");
	ec.clear();
	socket.write_some(boost::asio::buffer(msg), ec);
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		return EXIT_FAILURE;
	}

	// Read.
	char buf[512];
	ec.clear();
	size_t len = socket.read_some(boost::asio::buffer(buf), ec);
	if (ec)
	{
		std::cout << ec.message() << std::endl;
		return EXIT_FAILURE;
	}
	std::cout << std::string(buf, len) << std::endl;

	return 0;
}