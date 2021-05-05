#pragma once

#include "proxy/Types.h"

namespace proxy
{

class IProxy
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~IProxy() { }

	//
	// Public interface
	//
public:
	//! Starts.
	virtual void Start(const unsigned short port) = 0;
	//! Stops.
	virtual void Stop() = 0;
};

}