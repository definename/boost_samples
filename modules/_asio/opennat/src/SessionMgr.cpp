#include "pch.h"
#include "nat/SessionMgr.h"
#include "nat/Session.h"

namespace opennat
{

SessionMgr::SessionMgr()
{ }

SessionMgr::~SessionMgr()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "Client manager destruction error: " << e.what();
	}
}

bool SessionMgr::AddSession(types::SessionPtr session)
{
	boost::mutex::scoped_lock l(mutex_);
	return sessionMap_.insert(std::make_pair(session->GetId(), session)).second;
}

bool SessionMgr::AddBan(const types::BannedList::value_type& id)
{
	boost::mutex::scoped_lock l(banMutex_);
	return bannedList_.emplace(id).second;
}

types::SessionMap::size_type SessionMgr::Count(const types::SessionMap::key_type& id)
{
	return sessionMap_.count(id);
}

types::SessionPtr SessionMgr::Find(const types::SessionMap::key_type& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.find(id);
	if (ret == sessionMap_.end())
		BOOST_THROW_EXCEPTION(std::runtime_error("Unable to find session with given id"));

	return ret->second;
}

bool SessionMgr::IsBanned(const types::BannedList::value_type& id)
{
	boost::mutex::scoped_lock l(banMutex_);
	return bannedList_.count(id) > 0;
}

bool SessionMgr::RemoveBan(const types::BannedList::value_type& id)
{
	boost::mutex::scoped_lock l(banMutex_);
	return bannedList_.erase(id) > 0;
}

void SessionMgr::RemoveSession(const types::SessionMap::key_type& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.find(id);
	if (ret == sessionMap_.end())
	{
		LERR_ << "Unable to find session with given id";
		return;
	}

	if (ret->second)
	{
		ret->second->Stop();
		ret->second.reset();
	}
	sessionMap_.erase(id);
	LDBG_ << "Session with id: " << boost::uuids::to_string(id) << " has been removed";
}

void SessionMgr::Stop()
{
	boost::mutex::scoped_lock l(mutex_);
	for (types::SessionMap::value_type& val : sessionMap_)
	{
		val.second->Stop();
		val.second.reset();
	}
	sessionMap_.clear();
	LDBG_ << "Session manager has been stopped";
}

}