#include "pch.h"
#include "proxy/ClientMgr.h"
#include "proxy/Session.h"

namespace proxy
{

ClientMgr::ClientMgr()
{ }

ClientMgr::~ClientMgr()
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

void ClientMgr::AddSession(types::SessionPtr session)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.insert(std::make_pair(session->GetId(), session));
	if (!ret.second)
		BOOST_THROW_EXCEPTION(std::runtime_error("Unable to add given session"));
}

types::SessionPtr ClientMgr::Find(const boost::uuids::uuid& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.find(id);
	if (ret == sessionMap_.end())
		BOOST_THROW_EXCEPTION(std::runtime_error("Unable to find session with given id"));

	return ret->second;
}

void ClientMgr::RemoveSession(const boost::uuids::uuid& id)
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

void ClientMgr::Stop()
{
	boost::mutex::scoped_lock l(mutex_);
	for (SessionMap::value_type& val : sessionMap_)
	{
		val.second->Stop();
		val.second.reset();
	}
	sessionMap_.clear();
	LDBG_ << "Client manager has been stopped";
}

}