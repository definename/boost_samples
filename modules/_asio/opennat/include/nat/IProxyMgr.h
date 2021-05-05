#pragma once

#include "nat/Types.h"

namespace opennat
{

class IProxyMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~IProxyMgr() { }

	//
	// Public interface.
	//
public:
	//! Adds session.
	virtual bool AddSession(types::SessionPtr session) = 0;
	//! Finds session pair with given id.
	virtual types::PairPtr FindPair(const boost::uuids::uuid& id) = 0;
	//! Removes session with given id
	virtual void RemoveSession(const boost::uuids::uuid& id) = 0;
};

}