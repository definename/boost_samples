#include "pch.h"
#include "Log.h"

int main(int argc, char* argv[])
{
	try
	{
		LTRC_ << "Hello log";
		LDBG_ << "Hello log";
		LAPP_ << "Hello log";
		LWRN_ << "Hello log";
		LERR_ << "Hello log";
		LFTL_ << "Hello log";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}