#include "pch.h"

#if 0
	Conficuration file structure:

	[Factory]
	apples = 10
	oranges = 20
#endif

void main(int argc, char* argv[])
{
	try
	{
		boost::program_options::options_description desc("All options");
		desc.add_options()
			("Factory.apples", boost::program_options::value<int>(), "Apples that you have")
			("Factory.oranges", boost::program_options::value<int>(), "Oranges that you have")
			("Factory.name", boost::program_options::value<std::string>(), "Oranges name");

		boost::program_options::variables_map vm;
		try
		{
			boost::program_options::store(
				boost::program_options::parse_config_file<char>("conf.cfg", desc), vm);
		}
		catch (const boost::program_options::error& e)
		{
			std::cerr << "Failed to parse command line: " << e.what() << std::endl;
		}

		boost::program_options::notify(vm);
		if (vm.count("Factory.apples")) {
			std::cout << "We have: " << vm["Factory.apples"].as<int>() << " apples" << std::endl;
		}
		if (vm.count("Factory.oranges")) {
			std::cout << "We have: " << vm["Factory.oranges"].as<int>() << " apples" << std::endl;
		}
		if (vm.count("Factory.name")) {
			std::cout << "Fruit name: " << vm["Factory.name"].as<std::string>() << std::endl;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "Error occurred: " << e.what() << std::endl;
	}
}