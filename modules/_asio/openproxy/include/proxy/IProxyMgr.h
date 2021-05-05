#pragma once

#include "proxy/Types.h"

namespace proxy
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
	//! Adds session pair.
	virtual void AddPair(const boost::uuids::uuid& id, types::PairPtr pair) = 0;
	//! Adds session.
	virtual void AddSession(types::SessionPtr session) = 0;
	//! Finds session pair with given id.
	virtual types::PairPtr FindPair(const boost::uuids::uuid& id) = 0;
	//! Removes session with given id
	virtual void RemoveSession(const boost::uuids::uuid& id) = 0;
};

}