#pragma once

#include "IKernel.h"

namespace emulator
{

class Kernel
	: public IKernel
	, public boost::serialization::singleton<Kernel>
{
	//
	// Construction and destruction.
	//
private:
	//! Constructor.
	Kernel();
	//! Destructor.
	~Kernel();

	//
	// Private interface.
	//
private:
	//! Allows to access the member function.
	friend class boost::serialization::singleton<Kernel>;

	//
	// Public interface
	//
public:
	//! Initializes.
	void Init() const;
};

}