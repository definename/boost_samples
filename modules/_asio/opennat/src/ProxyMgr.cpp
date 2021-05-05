#include "pch.h"
#include "nat/ProxyMgr.h"
#include "nat/Session.h"

namespace opennat
{

ProxyMgr::ProxyMgr()
{ }

ProxyMgr::~ProxyMgr()
{ }

bool ProxyMgr::AddSession(types::SessionPtr session)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = proxyMap_.find(session->GetId());
	if (ret == proxyMap_.end())
	{
		types::PairPtr pair;
		if (session->GetType() == types::PROXYDOWN)
		{
			pair = boost::make_shared<types::SessionPair>(session, nullptr);
		}
		else if (session->GetType() == types::PROXYUP)
		{
			pair = boost::make_shared<types::SessionPair>(nullptr, session);
		}
		else
		{
			LERR_ << "Unknown session type";
			return false;
		}

		proxyMap_.insert(std::make_pair(session->GetId(), pair));
	}
	else if (session->GetType() == types::PROXYDOWN)
	{
		if (ret->second->down_)
		{
			LERR_ << "Down stream session has already initialized";
			return false;
		}
		ret->second->down_ = session;
	}
	else if (session->GetType() == types::PROXYUP)
	{
		if (ret->second->up_)
		{
			LERR_ << "Down stream session has already initialized";
			return false;
		}
		ret->second->up_ = session;
	}
	else
	{
		LERR_ << "Unknown session type";
		return false;
	}

	return true;
}

types::PairPtr ProxyMgr::FindPair(const boost::uuids::uuid& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = proxyMap_.find(id);
	if (ret == proxyMap_.end())
		BOOST_THROW_EXCEPTION(std::runtime_error("Unable to find session pair with given id"));

	return ret->second;
}

void ProxyMgr::RemoveSession(const boost::uuids::uuid& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = proxyMap_.find(id);
	if (ret == proxyMap_.end())
	{
		LERR_ << "Unable to find session pair with given id";
		return;
	}

	if (ret->second->down_)
		ret->second->down_->Stop();
	if (ret->second->up_)
		ret->second->up_->Stop();

	proxyMap_.erase(id);

	LDBG_ << "Session with id: " << boost::uuids::to_string(id) << " has been removed";
}

}