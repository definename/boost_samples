#include "pch.h"

int main(int argc, char *argv[])
{
	try
	{
		using namespace boost::interprocess;
		typedef std::pair<double, int> MyType;

		//Open managed shared memory
		managed_shared_memory segment(open_only, "MySharedMemory");

		std::pair<MyType*, managed_shared_memory::size_type> res;

		//Find the array
		res = segment.find<MyType>("MyType array");
		//Length should be 10
		if (res.second != 10)
		{
			std::cerr << "!10" << std::endl;
			return 1;
		}
		else
		{
			std::cout << res.second << std::endl;
		}

		//Find the object
		res = segment.find<MyType>("MyType instance");
		//Length should be 1
		if (res.second != 1)
		{
			std::cerr << "!1" << std::endl;
			return 1;
		}
		else
		{
			std::cout << res.second << std::endl;
		}

		//Find the array constructed from iterators
		res = segment.find<MyType>("MyType array from it");
		//Length should be 3
		if (res.second != 3)
		{
			std::cerr << "!3" << std::endl;
			return 1;
		}
		else
		{
			std::cout << res.second << std::endl;
		}

		//We're done, delete all the objects
		segment.destroy<MyType>("MyType array");
		segment.destroy<MyType>("MyType instance");
		segment.destroy<MyType>("MyType array from it");
	}
	catch (const boost::interprocess::interprocess_exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}