#pragma once

#include "proxy/Types.h"

namespace proxy
{

class IClientMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~IClientMgr() { }

	//
	// Public interface.
	//
public:
	//! Adds session.
	virtual void AddSession(types::SessionPtr session) = 0;
	//! Finds session with given id.
	virtual types::SessionPtr Find(const boost::uuids::uuid& id) = 0;
	//! Removes session with given id
	virtual void RemoveSession(const boost::uuids::uuid& id) = 0;
	//! Stops.
	virtual void Stop() = 0;
};

}