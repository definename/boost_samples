#pragma once

#include "proxy/Types.h"

namespace proxy
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
	//! Returns read buffer.
	virtual types::Buff& GetBuffRead() = 0;
	//! Returns write buffer.
	virtual types::Buff& GetBuffWrite() = 0;
	//! Returns read bytes.
	virtual size_t GetBytesReadBuff() const = 0;
	//! Returns write bytes.
	virtual size_t GetBytesWriteBuff() const = 0;
	//! Returns client id.
	virtual boost::uuids::uuid GetClientId() const = 0;
	//! Returns id.
	virtual boost::uuids::uuid GetId() const = 0;
	//! Returns ready status.
	virtual bool GetReady() const = 0;
	//! Returns socket pointer.
	virtual types::SocketPtr GetSocket() const = 0;
	//! Returns socket type.
	virtual protocol::StreamType GetType() const = 0;
	//! Sets transferred bytes.
	virtual void SetBytesReadBuff(const size_t size) = 0;
	//! Sets transferred bytes to write buff.
	virtual void SetBytesWriteBuff(const size_t size) = 0;
	//! Sets client id.
	virtual void SetClientId(const boost::uuids::uuid& id) = 0;
	//! Sets id.
	virtual void SetId(const boost::uuids::uuid& id) = 0;
	//! Sets ready status.
	virtual void SetReady(const bool ready) = 0;
	//! Sets socket type.
	virtual void SetType(const protocol::StreamType type) = 0;
	//! Stops session.
	virtual void Stop() = 0;
};

}