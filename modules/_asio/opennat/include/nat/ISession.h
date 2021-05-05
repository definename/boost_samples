#pragma once

#include "nat/Types.h"

namespace opennat
{

class ISession
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~ISession() { }

	//
	// Public interface.
	//
public:
	//! Returns transferred bytes.
	virtual std::size_t GetBytesTransferred() const = 0;
	//! Returns deadline timer pointer.
	virtual types::TimerPtr GetDeadline() = 0;
	//! Returns id.
	virtual boost::uuids::uuid GetId() const = 0;
	//! Returns remote client id.
	virtual boost::uuids::uuid GetRemoteId() const = 0;
	//! Returns socket pointer.
	virtual types::SocketPtr GetSocket() const = 0;
	//! Returns buffer.
	virtual types::StreamBuff& GetStream() = 0;
	//! Returns timeout value.
	virtual unsigned int& GetTimeout() = 0;
	//! Returns timer pointer.
	virtual types::TimerPtr GetTimer() = 0;
	//! Returns session type.
	virtual types::SessionType GetType() const = 0;
	//! Is session ready.
	virtual bool IsReady() const = 0;
	//! Sets transferred bytes.
	virtual void SetBytesTransferred(const std::size_t bytes) = 0;
	//! Sets id.
	virtual void SetId(const boost::uuids::uuid& id) = 0;
	//! Sets remote client id.
	virtual void SetRemoteId(const boost::uuids::uuid& id) = 0;
	//! Sets ready flag.
	virtual void SetReady(const bool ready) = 0;
	//! Sets session type.
	virtual void SetType(const types::SessionType type) = 0;
	//! Stops session.
	virtual void Stop() = 0;
};

}