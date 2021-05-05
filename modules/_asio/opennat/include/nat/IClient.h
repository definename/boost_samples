#pragma once

#include "nat/Types.h"

namespace opennat
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
	typedef boost::signals2::signal<void(
		const boost::uuids::uuid& sessionId,
		const types::SessionType type)> OnConnected;
	//! Error signal type.
	typedef boost::signals2::signal<void(
		const boost::uuids::uuid& sessionId,
		const types::SessionType type)> OnError;
	//! Receive signal type.
	typedef boost::signals2::signal<void(
		const boost::uuids::uuid& sessionId,
		const types::Buff::value_type* data,
		const std::size_t bytes)> OnReceive;
	//! Request client signal type.
	typedef boost::signals2::signal<void(
		const boost::uuids::uuid& clientId,
		const bool connected)> OnSubscription;

	//
	// Public interface
	//
public:
	//! Connects asynchronously.
	virtual void AsyncConnect(const boost::asio::ip::tcp::endpoint ep, const boost::uuids::uuid& id) = 0;
	//! Binds to the given port.
	virtual void Bind(const unsigned short port = 0) = 0;
	//! Connects session.
	virtual void Connect(const boost::asio::ip::tcp::endpoint ep, const boost::uuids::uuid& id) = 0;
	//! Is client connected to the server.
	virtual bool IsConnected() const = 0;
	//! Create direct session.
	virtual void Direct(const boost::uuids::uuid& clientId, const boost::uuids::uuid& sessionId) = 0;
	//! Disconnects client.
	virtual void Disconnect() = 0;
	//! Sets OnConnected signal.
	virtual boost::signals2::connection DoOnConnected(const OnConnected::slot_type& signal) = 0;
	//! Sets OnError signal.
	virtual boost::signals2::connection DoOnError(const OnError::slot_type& signal) = 0;
	//! Sets OnReceive signal.
	virtual boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal) = 0;
	//! Sets OnReceiveP2P signal.
	virtual boost::signals2::connection DoOnReceiveP2P(const OnReceive::slot_type& signal) = 0;
	//! Sets OnSubscription signal.
	virtual boost::signals2::connection DoOnSubscription(const OnSubscription::slot_type& signal) = 0;
	//! Returns client id.
	virtual boost::uuids::uuid GetId() const = 0;
	//! Returns config pointer.
	virtual types::ClientCfgPtr GetConfig() const = 0;
	//! Opens NAT.
	virtual types::CommandPtr OpenNAT() = 0;
	//! Create proxy session.
	virtual void Proxy(types::SessionConfigPtr config) = 0;
	//! Sends custom data to session (direct, proxy) with given id.
	virtual void Send(const boost::uuids::uuid& sessionId, types::MessagePtr msg) = 0;
	//! Sends custom data to client with given id (through server).
	virtual void Send2Client(const boost::uuids::uuid& clientId, types::MessagePtr msg) = 0;
	//! Sends custom data to server.
	virtual void Send2Server(types::MessagePtr msg) = 0;
	//! Starts UPNP part of client.
	virtual void StartUPNP() = 0;
	//! Stops.
	virtual void Stop() = 0;
	//! Subscribe.
	virtual void Subscribe(const boost::uuids::uuid& clientId) = 0;
};

}