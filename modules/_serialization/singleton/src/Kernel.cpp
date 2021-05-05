#include "pch.h"
#include "Kernel.h"

namespace emulator
{

Kernel::Kernel()
{ }

Kernel::~Kernel()
{ }

void Kernel::Init() const
{
	std::cout << "Kernel has been initialized" << std::endl;
}

}