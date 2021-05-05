#pragma once
#include "proxy/ISession.h"

namespace proxy
{

class Session 
	: public ISession
	, private boost::noncopyable
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	~Session();
	
	//
	// Construction and destruction.
	//
private:
	//! Constructor.
	Session(boost::asio::io_service& ioService);

	//
	// Public interface.
	//
public:
	//! Constructs session.
	static types::SessionPtr Construct(boost::asio::io_service& ioService);
	//! Returns read buffer.
	types::Buff& GetBuffRead();
	//! Returns write buffer.
	types::Buff& GetBuffWrite();
	//! Returns transferred bytes to read buff.
	size_t GetBytesReadBuff() const;
	//! Returns transferred bytes to write buff.
	size_t GetBytesWriteBuff() const;
	//! Returns client id.
	boost::uuids::uuid GetClientId() const;
	//! Returns id.
	boost::uuids::uuid GetId() const;
	//! Returns ready status.
	bool GetReady() const;
	//! Returns socket pointer.
	types::SocketPtr GetSocket() const;
	//! Returns socket type.
	protocol::StreamType GetType() const;
	//! Sets transferred bytes to read buff.
	void SetBytesReadBuff(const size_t size);
	//! Sets transferred bytes to write buff.
	void SetBytesWriteBuff(const size_t size);
	//! Sets client id.
	void SetClientId(const boost::uuids::uuid& id);
	//! Sets id.
	void SetId(const boost::uuids::uuid& id);
	//! Sets ready status.
	void SetReady(const bool ready);
	//! Sets socket type.
	void SetType(const protocol::StreamType type);
	//! Stops
	void Stop();

	//
	// Private data members.
	//
private:
	//! Read buffer.
	types::Buff buffRead_;
	//! Write buffer.
	types::Buff buffWrite_;
	//! Read bytes.
	size_t bytesRead_;
	//! Write bytes bytes.
	size_t bytesWrite_;
	//! Client id.
	boost::uuids::uuid clientId_;
	//! Session id.
	boost::uuids::uuid id_;
	//! Is session ready.
	bool isReady_;
	//! Socket pointer.
	types::SocketPtr socket_;
	//! Socket type.
	protocol::StreamType type_;
};

}