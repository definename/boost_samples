#include "pch.h"

int main(int argc, TCHAR* argv[])
{
	try
	{
		boost::asio::ip::tcp::resolver::query query(
			boost::asio::ip::tcp::v4(),
			boost::asio::ip::host_name(),
			"21042");
		boost::asio::io_service ioService;
		boost::asio::ip::tcp::resolver resolver(ioService);
		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

		boost::asio::ip::address_v4 addr = it->endpoint().address().to_v4();
		boost::asio::ip::address_v4 netmask = boost::asio::ip::address_v4::netmask(addr);
		boost::asio::ip::address_v4 network = boost::asio::ip::address_v4(
			addr.to_ulong() & netmask.to_ulong());


		std::cout << addr.to_string() << std::endl;
		std::cout << netmask.to_string() << std::endl;
		std::cout << network.to_string() << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}