#pragma once

#include "nat/Types.h"

namespace opennat
{

class IServer
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~IServer() { }

	//
	// Public types.
	//
public:
	//! Disconnected signal type.
	typedef boost::signals2::signal<void(const boost::uuids::uuid& sessionId)> OnDisconnected;
	//! Receive signal type.
	typedef boost::signals2::signal<void(
		const boost::uuids::uuid& sessionId,
		const types::Buff::value_type* data,
		const std::size_t bytes)> OnReceive;

	//
	// Public interface
	//
public:
	//! Sets OnDisconnected signal.
	virtual boost::signals2::connection DoOnDisconnected(const OnDisconnected::slot_type& signal) = 0;
	//! Sets OnReceive signal.
	virtual boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal) = 0;
	//! Starts.
	virtual void Start(const unsigned short port) = 0;
	//! Stops.
	virtual void Stop() = 0;
};

}