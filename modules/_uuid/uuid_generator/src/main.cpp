#include "pch.h"

boost::uuids::uuid TestNameGenerator(const std::string& name);

int main(int argc, char* argv[])
{
	try
	{
		//! Tests name generator.
		std::cout << TestNameGenerator("name") << std::endl;
		std::cout << TestNameGenerator("name") << std::endl;
		std::cout << TestNameGenerator("NAME") << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}

boost::uuids::uuid TestNameGenerator(const std::string& name)
{
	boost::uuids::name_generator gen(
		boost::uuids::string_generator()("6ba7b810-9dad-11d1-80b4-00c04fd430c8"));

	return gen(name);
}