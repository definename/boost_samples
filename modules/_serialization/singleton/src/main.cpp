#include "pch.h"
#include "Emulator.h"

int main()
{
	try
	{
		emulator::GetKernel().Init();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what();
	}

	return 0;
}