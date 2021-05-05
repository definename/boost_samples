#pragma once

#include "proxy/IProxyMgr.h"

namespace proxy
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
	// Public types.
	//
public:
	//! Session pair map type.
	typedef boost::unordered_map<boost::uuids::uuid, types::PairPtr> SessionMap;

	//
	// Public interface.
	//
public:
	//! Adds session pair.
	void AddPair(const boost::uuids::uuid& id, types::PairPtr pair);
	//! Adds session.
	void AddSession(types::SessionPtr session);
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
	//! Sessions' map.
	SessionMap sessionMap_;
};

}