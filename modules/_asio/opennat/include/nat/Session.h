#pragma once

#include "nat/ISession.h"

namespace opennat
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
	// Public types
	//
public:
	//! Constant type.
	enum Constants
	{
		MAXTIMEOUT = 3,
		DEADLINE = 5
	};
	
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
	//! Returns transferred bytes.
	std::size_t GetBytesTransferred() const override;
	//! Returns deadline timer pointer.
	types::TimerPtr GetDeadline() override;
	//! Returns id.
	boost::uuids::uuid GetId() const override;
	//! Returns remote client id.
	boost::uuids::uuid GetRemoteId() const override;
	//! Returns socket pointer.
	types::SocketPtr GetSocket() const override;
	//! Returns buffer.
	types::StreamBuff& GetStream() override;
	//! Returns session type.
	types::SessionType GetType() const override;
	//! Returns timeout value.
	unsigned int& GetTimeout() override;
	//! Returns timer pointer.
	types::TimerPtr GetTimer() override;
	//! Is session ready.
	bool IsReady() const override;
	//! Sets transferred bytes.
	void SetBytesTransferred(const std::size_t bytes) override;
	//! Sets id.
	void SetId(const boost::uuids::uuid& id) override;
	//! Sets remote id.
	void SetRemoteId(const boost::uuids::uuid& id) override;
	//! Sets ready flag.
	void SetReady(const bool ready) override;
	//! Sets session type.
	void SetType(const types::SessionType type) override;
	//! Stops
	void Stop() override;

	//
	// Private data members.
	//
private:
	//! Stream buffer.
	types::StreamBuff buffer_;
	//! Bytes transferred.
	std::size_t bytes_;
	//! Deadline timer.
	types::TimerPtr deadline_;
	//! Session id.
	boost::uuids::uuid id_;
	//! Remote client id.
	boost::uuids::uuid remoteId_;
	//! Is session ready.
	bool isReady_;
	//! Socket pointer.
	types::SocketPtr socket_;
	//! Session type.
	types::SessionType type_;
	//! Timer pointer type.
	types::TimerPtr timer_;
	//! Current timeout.
	unsigned int timeout_;
};

}