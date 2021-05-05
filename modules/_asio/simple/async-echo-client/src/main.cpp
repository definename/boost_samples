#include "pch.h"
#include "EchoClient.h"

int main()
{
	try
	{
		boost::asio::io_service io;
		boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string("127.0.0.1"), 2001);
		EchoClient::Start(ep, "Hello", io);
		io.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}
	return 0;
}