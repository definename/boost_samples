#pragma once

#include "nat/ISessionMgr.h"

namespace opennat
{

class SessionMgr : public ISessionMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	SessionMgr();
	//! Destructor.
	~SessionMgr();

	//
	// Public interface.
	//
public:
	//! Adds session.
	bool AddSession(types::SessionPtr session);
	//! Add session to banned list.
	bool AddBan(const types::BannedList::value_type& id);
	//! Count sessions with given id.
	types::SessionMap::size_type Count(const types::SessionMap::key_type& id);
	//! Is session banned.
	bool IsBanned(const types::BannedList::value_type& id);
	//! Finds session with given id.
	types::SessionPtr Find(const types::SessionMap::key_type& id);
	//! Removes session with given id
	void RemoveSession(const types::SessionMap::key_type& id);
	//! Removed session from banned list.
	bool RemoveBan(const types::BannedList::value_type& id);
	//! Stops.
	void Stop();

	//
	// Private data members.
	//
private:
	//! Mutex of banned list.
	boost::mutex banMutex_;
	//! List of banned sessions.
	types::BannedList bannedList_;
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! Sessions' map.
	types::SessionMap sessionMap_;
};

}