#include "pch.h"
#include "IPCClient.h"

void Loop();

void HandleReceive(const std::string& data);

int main(int argc, char* argv[])
{
	try
	{
		boost::program_options::options_description desc("All options");
		desc.add_options()
			("help", "Produce help message")
			("create,c", "Create message queue mode")
			("open,o", "Open message queue mode");

		boost::program_options::variables_map vm;
		try
		{
			boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc), vm);
		}
		catch (const boost::program_options::error& e)
		{
			std::cerr << "Failed to parse command line: " << e.what();
		}

		boost::program_options::notify(vm);
		if (vm.count("help") || argc < 2 || vm.empty())
		{
			std::stringstream stream;
			desc.print(stream);
			std::cerr << stream.str();

			return EXIT_FAILURE;
		}
		else if (vm.count("create") > 0)
		{
			boost::shared_ptr<ipc::IPCClient> create(new ipc::IPCClient("tray_queue"));
			create->Create();
			create->Send({ "Hello" });

			Loop();
		}
		else if (vm.count("open") > 0)
		{
			boost::shared_ptr<ipc::IPCClient> open(new ipc::IPCClient("tray_queue"));
			open->Open();
			open->DoOnReceive(boost::bind(HandleReceive, _1));

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

void HandleReceive(const std::string& data)
{
	std::cout << "Received: " << data << std::endl;
}