#pragma once

#include "nat/Types.h"

namespace opennat
{

class ISubscribeMgr
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~ISubscribeMgr() { }

	//
	// Public types.
	//
public:
	//! OnNotify signal type.
	typedef boost::signals2::signal<void(
		const boost::uuids::uuid& sessionId,
		const types::SessionType type,
		types::CommandPtr cmd)> OnNotify;

	//
	// Public interface.
	//
public:
	//! Sets OnNotify signal.
	virtual boost::signals2::connection DoOnNotify(const OnNotify::slot_type& signal) = 0;
	//! Subscribe.
	virtual void Subscribe(
		const boost::uuids::uuid& subscription,
		const boost::uuids::uuid& subscriber,
		const bool isOnline) = 0;
	//! Notify.
	virtual void Notify(const boost::uuids::uuid& subscription, const bool isOnline) = 0;
};

}