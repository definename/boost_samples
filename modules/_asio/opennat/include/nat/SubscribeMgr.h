#pragma once

#include "nat/ISubscribeMgr.h"

namespace opennat
{

class SubscribeMgr : public ISubscribeMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	SubscribeMgr();
	//! Destructor.
	~SubscribeMgr();

	//
	// Public types.
	//
public:
	//! Subscribe list type.
	typedef boost::unordered_map<
		boost::uuids::uuid,
		boost::unordered_set<boost::uuids::uuid> > SubscribeList;

	//
	// Public interface.
	//
public:
	//! Sets OnNotify signal.
	boost::signals2::connection DoOnNotify(const OnNotify::slot_type& signal);
	//! Subscribe.
	void Subscribe(
		const boost::uuids::uuid& subscription,
		const boost::uuids::uuid& subscriber,
		const bool isOnline);
	//! Notify.
	void Notify(const boost::uuids::uuid& subscription, const bool isOnline);

	//
	// Private data members.
	//
private:
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! Subscribe list.
	SubscribeList subscribeList_;
	//! OnNotify signal.
	OnNotify onNotify_;
};

}