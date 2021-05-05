#include "pch.h"
#include "proxy/ProxyFactory.h"
#include "proxy/Proxy.h"
#include "proxy/Client.h"

// Usage:
// *.exe --proxy
// *.exe -p

// *.exe --client=up
// *.exe -c up

// *.exe --client=down
// *.exe -c down


//! Starts loop.
void Loop();

int main(int argc, char* argv[])
{
	try
	{
		boost::program_options::options_description desc("All options");
		desc.add_options()
			("help", "Produce help message")
			("client,c", boost::program_options::value<std::string>(), "Client mode")
			("proxy,p", "Proxy mode");

		boost::program_options::variables_map vm;
		try
		{
			boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc), vm);
		}
		catch (const boost::program_options::error& e)
		{
			std::cerr << "Failed to parse command line: " << e.what() << std::endl;
		}

		boost::program_options::notify(vm);
		if (vm.count("help") || argc < 2 || vm.empty())
		{
			std::cout << desc << std::endl;
			return EXIT_FAILURE;
		}

		boost::shared_ptr<proxy::ProxyFactory> factory(new proxy::ProxyFactory());
		if (vm.count("client") > 0)
		{
			std::string mode = vm["client"].as<std::string>();

			proxy::types::ClientPtr client;
			if (boost::iequals(mode, "up"))
			{
				client = factory->GetClient(
					proxy::protocol::UPSTREAM,
					boost::uuids::random_generator()());
			}
			else if (boost::iequals(mode, "down"))
			{
				client = factory->GetClient(
					proxy::protocol::DOWNSREAM,
					boost::uuids::random_generator()());
			}
			else
			{
				std::cout << desc << std::endl;
				return EXIT_FAILURE;
			}

			client->DoOnConnected([](const boost::uuids::uuid& sessionId) -> void {
				LDBG_ << "Client connected to session: " << boost::uuids::to_string(sessionId);
			});
			client->DoOnError([](const boost::uuids::uuid& sessionId) -> void {
				LERR_ << "Client session error: " << boost::uuids::to_string(sessionId);
			});
			client->DoOnReady([](const boost::uuids::uuid& sessionId) -> void {
				LDBG_ << "Session is ready: " << boost::uuids::to_string(sessionId);
			});
			client->DoOnReceive([](const std::string& data, const boost::uuids::uuid& sessionId) -> void {
				LDBG_ << "Data from session: " << boost::uuids::to_string(sessionId);
			});

			client->Connect(
				boost::uuids::string_generator()("dc5bc054-f368-11e6-bc64-92361f002671"),
				boost::asio::ip::tcp::endpoint(
				boost::asio::ip::address::from_string("127.0.0.1"), proxy::protocol::proxyPort));

			std::cout << mode << " client has been started" << std::endl;

			Loop();
		}
		else if (vm.count("proxy") > 0)
		{
			auto proxy = factory->GetProxy();
			proxy->Start(proxy::protocol::proxyPort);

			Loop();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

void Loop()
{
	MSG msg;
	while (!GetMessage(&msg, NULL, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}