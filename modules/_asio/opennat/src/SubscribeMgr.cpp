#include "pch.h"
#include "nat/SubscribeMgr.h"
#include "Command.pb.h"

namespace opennat
{

SubscribeMgr::SubscribeMgr()
{ }

SubscribeMgr::~SubscribeMgr()
{ }

boost::signals2::connection SubscribeMgr::DoOnNotify(const OnNotify::slot_type& signal)
{
	return onNotify_.connect(signal);
}

void SubscribeMgr::Subscribe(
	const boost::uuids::uuid& subscription,
	const boost::uuids::uuid& subscriber,
	const bool isOnline)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = subscribeList_.find(subscription);
	if (ret == subscribeList_.end())
	{
		boost::unordered_set<boost::uuids::uuid> list;
		list.insert(subscriber);
		subscribeList_.insert(std::make_pair(subscription, list));
	}
	else
	{
		ret->second.insert(subscriber);
	}

	opennat::types::CommandPtr cmd(new opennat::Command());
	cmd->set_protocol(opennat::Command_Protocol_Subscribe);

	auto subscribe = cmd->mutable_subscribe();
	subscribe->set_protocol(opennat::Subscribe_Protocol_Reply);
	subscribe->set_online(isOnline);
	subscribe->set_client_id(boost::uuids::to_string(subscription));

	onNotify_(subscriber, opennat::types::STUN, cmd);
}

void SubscribeMgr::Notify(const boost::uuids::uuid& subscription, const bool isOnline)
{
	boost::mutex::scoped_lock l(mutex_);
	auto ret = subscribeList_.find(subscription);
	if (ret != subscribeList_.end())
	{
		for (const auto& val : ret->second)
		{
			opennat::types::CommandPtr cmd(new opennat::Command());
			cmd->set_protocol(opennat::Command_Protocol_Subscribe);

			auto subscribe = cmd->mutable_subscribe();
			subscribe->set_protocol(opennat::Subscribe_Protocol_Reply);

			subscribe->set_online(isOnline);
			subscribe->set_client_id(boost::uuids::to_string(subscription));

			onNotify_(val, opennat::types::STUN, cmd);
		}
	}
}

}