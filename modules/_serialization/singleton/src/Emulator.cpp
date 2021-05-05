#include "pch.h"
#include "Kernel.h"

namespace emulator
{

const IKernel& GetKernel()
{
	return Kernel::get_const_instance();
}

}