#include "pch.h"
#include "proxy/ProxyMgr.h"
#include "proxy/Session.h"

namespace proxy
{

ProxyMgr::ProxyMgr()
{ }

ProxyMgr::~ProxyMgr()
{ }

void ProxyMgr::AddPair(const boost::uuids::uuid& id, types::PairPtr pair)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.insert(std::make_pair(id, pair));
	if (!ret.second)
		BOOST_THROW_EXCEPTION(std::runtime_error("Session with given id is already in list"));
}

void ProxyMgr::AddSession(types::SessionPtr session)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.find(session->GetId());
	if (ret == sessionMap_.end())
	{
		types::PairPtr pair;
		if (session->GetType() == protocol::DOWNSREAM)
			pair = boost::make_shared<types::SessionPair>(session, nullptr);
		else if (session->GetType() == protocol::UPSTREAM)
			pair = boost::make_shared<types::SessionPair>(nullptr, session);
		else
			BOOST_THROW_EXCEPTION(std::runtime_error("Unknown session type"));

		sessionMap_.insert(std::make_pair(session->GetId(), pair));
	}
	else if (session->GetType() == protocol::DOWNSREAM)
	{
		if (ret->second->down_)
			BOOST_THROW_EXCEPTION(std::runtime_error("Down stream session has alredy initilized"));
		ret->second->down_ = session;
	}
	else if (session->GetType() == protocol::UPSTREAM)
	{
		if (ret->second->up_)
			BOOST_THROW_EXCEPTION(std::runtime_error("Up stream session has alredy initilized"));
		ret->second->up_ = session;
	}
	else
		BOOST_THROW_EXCEPTION(std::runtime_error("Unknown session type"));
}

types::PairPtr ProxyMgr::FindPair(const boost::uuids::uuid& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.find(id);
	if (ret == sessionMap_.end())
		BOOST_THROW_EXCEPTION(std::runtime_error("Unable to find session pair with given id"));

	return ret->second;
}

void ProxyMgr::RemoveSession(const boost::uuids::uuid& id)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = sessionMap_.find(id);
	if (ret == sessionMap_.end())
	{
		LERR_ << "Unable to find session pair with given id";
		return;
	}

	if (ret->second->down_)
		ret->second->down_->Stop();
	if (ret->second->up_)
		ret->second->up_->Stop();

	sessionMap_.erase(id);

	LDBG_ << "Session with id: " << boost::uuids::to_string(id) << " has been removed";
}

}