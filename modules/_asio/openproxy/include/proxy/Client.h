#pragma once

#include "proxy/IClient.h"
#include "proxy/Core.h"

namespace proxy
{

class Client
	: public IClient
	, public Core
	, private boost::noncopyable
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Client(
		types::ClientMgrPtr clientMgr,
		const protocol::StreamType type,
		const boost::uuids::uuid& id);
	//! Destructor.
	~Client();

	//
	// Public interface
	//
public:
	//! Sets OnConnected signal.
	boost::signals2::connection DoOnConnected(const OnConnected::slot_type& signal);
	//! Sets OnError signal.
	boost::signals2::connection DoOnError(const OnError::slot_type& signal);
	//! Sets OnReady signal.
	boost::signals2::connection DoOnReady(const OnReady::slot_type& signal);
	//! Sets OnReceive signal.
	boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal);
	//! Connects client session.
	void Connect(
		const boost::uuids::uuid& sessionId,
		const boost::asio::ip::tcp::endpoint ep);
	//! Disconnects client.
	void Disconnect();
	//! Is session connected.
	bool IsConnected(const boost::uuids::uuid& sessionId);
	//! Sends.
	void Send(const boost::uuids::uuid& sessionId, protocol::DataPtr data);

	//
	// Public types.
	//
private:
	//! Queue pair type.
	typedef std::pair<boost::uuids::uuid, protocol::DataPtr> QueuePair;
	//! Send queue type.
	typedef std::queue<QueuePair> SendQueue;

	//
	// Private interface.
	//
private:
	//! Connects.
	void DoConnect(
		types::SessionPtr session,
		const boost::asio::ip::tcp::endpoint& endpoint);
	//! Do send data.
	void DoSend(types::SessionPtr session, protocol::DataPtr data);
	//! Logins.
	void Login(types::SessionPtr session);
	//! Receives.
	void Receive(types::SessionPtr session);
	//! Receives ready request.
	void ReceiveReady(types::SessionPtr session);
	//! Sends ready request.
	void SendReady(types::SessionPtr session);

	//
	// Private data members.
	//
private:
	//! Proxy client id.
	boost::uuids::uuid clientId_;
	//! Client manager pointer.
	types::ClientMgrPtr clientMgr_;
	//! Resolver pointer.
	types::ResolverPtr resolver_;
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! Connected signal.
	OnConnected onConnected_;
	//! Error signal.
	OnError onError_;
	//! Ready signal.
	OnReady onReady_;
	//! Receive signal.
	OnReceive onReceive_;
	//! Send queue.
	SendQueue sendQueue_;
	//! Socket type.
	protocol::StreamType type_;
};

}