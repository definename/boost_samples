#pragma once

#include "nat/IProxyMgr.h"

namespace opennat
{

class ProxyMgr : public IProxyMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	ProxyMgr();
	//! Destructor.
	~ProxyMgr();

	//
	// Public interface.
	//
public:
	//! Adds session.
	bool AddSession(types::SessionPtr session);
	//! Removes session with given id
	void RemoveSession(const boost::uuids::uuid& id);
	//! Finds session pair with given id.
	types::PairPtr FindPair(const boost::uuids::uuid& id);

	//
	// Private data members.
	//
private:
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! Proxy sessions' map.
	types::ProxyMap proxyMap_;
};

}