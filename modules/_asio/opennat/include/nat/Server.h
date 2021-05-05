#pragma once

#include "nat/IServer.h"
#include "nat/Core.h"

namespace opennat
{

class Server
	: public IServer
	, public Core
	, private boost::noncopyable
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Server(
		types::SessionMgrPtr sessionMgr, 
		types::ProxyMgrPtr proxyMgr,
		types::SubscribeMgrPtr subscribeMgr);
	//! Destructor.
	~Server();

	//
	// Public interface
	//
public:
	//! Sets OnDisconnected signal.
	boost::signals2::connection DoOnDisconnected(const OnDisconnected::slot_type& signal);
	//! Sets OnReceive signal.
	boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal);
	//! Starts.
	void Start(const unsigned short port);
	//! Stops.
	void Stop();

	//
	// Private interface.
	//
private:
	//! Accepts new connection.
	void Accept();
	//! Do send.
	void DoSend(types::SessionPtr session, types::MessagePtr message);
	//! Handles stun timeout.
	void HandleStunTimeout(
		const boost::system::error_code& ec,
		const boost::uuids::uuid& sessionId);
	//! Receives header/body only.
	void Receive(const types::SessionPtr session);
	//! Receives header/body only.
	void ReceiveProxy(const types::SessionPtr session);
	//! Sends.
	void Send(
		const boost::uuids::uuid& sessionId,
		const types::SessionType type,
		types::CommandPtr cmd);
	//! Sends p2p.
	void SendP2P(const types::SessionPtr from, const boost::uuids::uuid clientId);
	//! Sends proxy.
	void SendProxy(const types::SessionPtr from, const types::SessionPtr to);
	//! Process.
	bool Process(types::SessionPtr session, types::CommandPtr cmd);

	//
	// Private data members.
	//
private:
	//! Acceptor pointer.
	types::AcceptorPtr acceptor_;
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! Server queue.
	types::ServerQueue serverQueue_;
	//! Proxy manager pointer.
	types::ProxyMgrPtr proxyMgr_;
	//! Stun manager pointer.
	types::SessionMgrPtr stunMgr_;
	//! Subscribe manager.
	types::SubscribeMgrPtr subscribeMgr_;
	//! Receive signal.
	OnReceive onReceive_;
	//! Disconnected signal.
	OnDisconnected onDisconnected_;
};

}