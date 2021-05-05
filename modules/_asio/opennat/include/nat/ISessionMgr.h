#pragma once

#include "nat/Types.h"

namespace opennat
{

class ISessionMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~ISessionMgr() { }

	//
	// Public interface.
	//
public:
	//! Adds session.
	virtual bool AddSession(types::SessionPtr session) = 0;
	//! Add session to banned list.
	virtual bool AddBan(const types::BannedList::value_type& id) = 0;
	//! Count sessions with given id.
	virtual types::SessionMap::size_type Count(const types::SessionMap::key_type& id) = 0;
	//! Is session banned.
	virtual bool IsBanned(const types::BannedList::value_type& id) = 0;
	//! Finds session with given id.
	virtual types::SessionPtr Find(const types::SessionMap::key_type& id) = 0;
	//! Removes session with given id
	virtual void RemoveSession(const types::SessionMap::key_type& id) = 0;
	//! Removed session from banned list.
	virtual bool RemoveBan(const types::BannedList::value_type& id) = 0;
	//! Stops.
	virtual void Stop() = 0;
};

}