#pragma once

#include "proxy/Types.h"

namespace proxy
{

class IClient
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~IClient() { }

	//
	// Public types.
	//
public:
	//! Connected signal type.
	typedef boost::signals2::signal<void(const boost::uuids::uuid& sessionId)> OnConnected;
	//! Error signal type.
	typedef boost::signals2::signal<void(const boost::uuids::uuid& sessionId)> OnError;
	//! Ready to connect signal type.
	typedef boost::signals2::signal<void(const boost::uuids::uuid& sessionId)> OnReady;
	//! Receive signal type.
	typedef boost::signals2::signal<void(
		const std::string& data, const boost::uuids::uuid& sessionId)> OnReceive;

	//
	// Public interface
	//
public:
	//! Connects client session.
	virtual void Connect(
		const boost::uuids::uuid& sessionId,
		const boost::asio::ip::tcp::endpoint ep) = 0;
	//! Disconnects client.
	virtual void Disconnect() = 0;
	//! Sets OnConnected signal.
	virtual boost::signals2::connection DoOnConnected(const OnConnected::slot_type& signal) = 0;
	//! Sets OnError signal.
	virtual boost::signals2::connection DoOnError(const OnError::slot_type& signal) = 0;
	//! Sets OnReady signal.
	virtual boost::signals2::connection DoOnReady(const OnReady::slot_type& signal) = 0;
	//! Sets OnReceive signal.
	virtual boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal) = 0;
	//! Is session connected.
	virtual bool IsConnected(const boost::uuids::uuid& sessionId) = 0;
	//! Sends.
	virtual void Send(const boost::uuids::uuid& sessionId, protocol::DataPtr data) = 0;
};

}