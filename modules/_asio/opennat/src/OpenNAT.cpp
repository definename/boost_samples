#include "pch.h"
#include "nat/Kernel.h"
#include "nat/opennat.h"

namespace opennat
{

const IKernel& GetKernel()
{
	return Kernel::get_const_instance();
}

}