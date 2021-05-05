#include "pch.h"
#include "ProxyServer.h"

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "");
	try
	{
		std::cout << char(1) << " - 1" << std::endl;
		std::cout << char(2) << " - 2" << std::endl;
		std::cout << char(3) << " - 3" << std::endl;
		std::cout << char(4) << " - 4" << std::endl;
		std::cout << char(5) << " - 5" << std::endl;

		if (argc < 4)
			throw std::runtime_error(
			"Invalid input parameters.\nUsage: .exe acceptorPort serverIp serverPort");

		//const unsigned short acceptorPort = 21031;
		const unsigned short acceptorPort = boost::lexical_cast<unsigned short>(argv[1]);
		//const std::string serverIp = "192.168.25.2";
		const std::string serverIp = boost::lexical_cast<std::string>(argv[2]);
		//const unsigned short serverPort = 139;
		const unsigned short serverPort = boost::lexical_cast<unsigned short>(argv[3]);

		ProxyServer proxy(acceptorPort, serverIp, serverPort);
		proxy.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}