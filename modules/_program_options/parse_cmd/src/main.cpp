#include "pch.h"

// Usage:
// *.exe --help
// *.exe --apples=10 --oranges=20
// *.exe -a 10 --o 20

void main(int argc, char* argv[])
{
	try
	{
		boost::program_options::options_description desc("All options");
		desc.add_options()
			("help", "Produce help message")
			("apples,a", boost::program_options::value<int>(), "Apples that you have")
			("oranges,o", boost::program_options::value<int>(), "Oranges that you have");

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
		if (vm.count("help") || argc < 2)
		{
			std::cout << desc << std::endl;
			return;
		}

		if (vm.count("apples"))
		{
			std::cout << "We have: " << vm["apples"].as<int>() << " apples" << std::endl;
		}
		if (vm.count("oranges"))
		{
			std::cout << "We have: " << vm["oranges"].as<int>() << " apples" << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error occurred: " << e.what() << std::endl;
	}
}