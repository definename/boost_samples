#pragma once

#include "nat/IClient.h"
#include "nat/Core.h"

namespace opennat
{

// Forward declaration.
class ConfigMgr;

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
	Client(const boost::uuids::uuid& id, types::SessionMgrPtr sessionMgr);
	//! Destructor.
	~Client();

	//
	// Public interface
	//
public:
	//! Connects asynchronously.
	void AsyncConnect(const boost::asio::ip::tcp::endpoint ep, const boost::uuids::uuid& id);
	//! Binds to the given port.
	void Bind(const unsigned short port = 0);
	//! Connects session.
	void Connect(const boost::asio::ip::tcp::endpoint ep, const boost::uuids::uuid& id);
	//! Is client connected to the server.
	bool IsConnected() const;
	//! Create direct session.
	void Direct(const boost::uuids::uuid& clientId, const boost::uuids::uuid& sessionId);
	//! Disconnects client.
	void Disconnect();
	//! Sets OnConnected signal.
	boost::signals2::connection DoOnConnected(const OnConnected::slot_type& signal);
	//! Sets OnError signal.
	boost::signals2::connection DoOnError(const OnError::slot_type& signal);
	//! Sets OnReceive signal.
	boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal);
	//! Sets OnReceiveP2P signal.
	boost::signals2::connection DoOnReceiveP2P(const OnReceive::slot_type& signal);
	//! Sets OnSubscription signal.
	boost::signals2::connection DoOnSubscription(const OnSubscription::slot_type& signal);
	//! Returns client id.
	boost::uuids::uuid GetId() const;
	//! Returns config pointer.
	types::ClientCfgPtr GetConfig() const;
	//! Opens NAT.
	types::CommandPtr OpenNAT();
	//! Create proxy session.
	void Proxy(types::SessionConfigPtr config);
	//! Sends custom data to session (direct, proxy, stun) with given id.
	void Send(const boost::uuids::uuid& sessionId, types::MessagePtr msg);
	//! Sends custom data to client with given id (through server).
	void Send2Client(const boost::uuids::uuid& clientId, types::MessagePtr msg);
	//! Sends custom data to server.
	void Send2Server(types::MessagePtr msg);
	//! Starts UPNP part of client.
	void StartUPNP();
	//! Stops.
	void Stop();
	//! Subscribe.
	void Subscribe(const boost::uuids::uuid& clientId);

	//
	// Private data members.
	//
private:
	//! Accepts.
	void Accept();
	//! Do direct session.
	void DoDirect(const types::SessionConfigPtr config);
	//! Do proxy session.
	void DoProxy(types::SessionConfigPtr config);
	//! Do send.
	void DoSend(types::SessionPtr session, types::MessagePtr message);
	//! Process.
	bool Process(types::SessionPtr session, types::CommandPtr cmd);
	//! Process p2p.
	void ProcessP2P(types::SessionPtr session, types::P2PPtr p2p);
	//! Receives.
	void Receive(types::SessionPtr session);
	//! Receives raw data.
	void ReceiveRaw(types::SessionPtr session);
	//! Sends.
	void Send(const boost::uuids::uuid& sessionId, types::CommandPtr cmd);
	//! Handles connection timeout.
	void HandleConnectionTimeout(
		const boost::system::error_code& ec,
		const boost::asio::ip::tcp::endpoint ep,
		const boost::uuids::uuid& id);
	//! Handles deadline timeout.
	void HandleDeadlineTimeout(const boost::system::error_code& ec, types::SessionPtr session);
	//! Handles stun timeout.
	void HandleStunTimeout(const boost::system::error_code& ec, const boost::uuids::uuid& sessionId);

	//
	// Private data members.
	//
private:
	//! Acceptor pointer.
	types::AcceptorPtr acceptor_;
	//! Client id.
	const boost::uuids::uuid clientId_;
	//! Connection mutex.
	boost::mutex connectMutex_;
	//! Connection queue.
	types::ConnectQueue connectQueue_;
	//! Config pointer.
	types::ClientCfgPtr config_;
	//! Connection timer.
	types::TimerPtr connectionTimer_;
	//! Connected signal.
	OnConnected onConnected_;
	//! Error signal.
	OnError onError_;
	//! Receive signal.
	OnReceive onReceive_;
	//! Receive p2p signal.
	OnReceive onReceiveP2P_;
	//! OnSubscription signal.
	OnSubscription onSubscription_;
	//! Resolver pointer.
	types::ResolverPtr resolver_;
	//! Session manager.
	types::SessionMgrPtr sessionMgr_;
	//! Send mutex.
	boost::mutex sendMutex_;
	//! Send queue.
	types::SendQueue sendQueue_;
	//! UPNP client pointer.
	types::UPNPClientPtr upnp_;
};

}