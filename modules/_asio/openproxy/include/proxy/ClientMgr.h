#pragma once

#include "proxy/IClientMgr.h"

namespace proxy
{

class ClientMgr : public IClientMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	ClientMgr();
	//! Destructor.
	~ClientMgr();

	//
	// Public types.
	//
public:
	//! Session map type.
	typedef boost::unordered_map<boost::uuids::uuid, types::SessionPtr> SessionMap;

	//
	// Public interface.
	//
public:
	//! Finds session with given id.
	types::SessionPtr Find(const boost::uuids::uuid& id);
	//! Adds session.
	void AddSession(types::SessionPtr session);
	//! Removes session with given id
	void RemoveSession(const boost::uuids::uuid& id);
	//! Stops.
	void Stop();

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