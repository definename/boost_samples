#pragma once

namespace emulator
{

class IKernel
{
	//
	// Construction and destruction.
	//
protected:
	//! Destructor.
	virtual ~IKernel() { }

	//
	// Public interface
	//
public:
	//! Initializes logging.
	virtual void Init() const = 0;

};

}