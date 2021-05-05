#pragma once

namespace opennat
{

// Forward declaration.
class IClient;
class Message;
class IServer;
class ISession;
class ISessionMgr;
class IProxyMgr;
class ISubscribeMgr;

class Command;
class P2P;
class MakeDirect;

class Errors;

class UPNPClient;

namespace types
{

// Header type.
typedef boost::array<unsigned char, sizeof(uint64_t)> Header;
// Buffer type.
typedef std::vector<unsigned char> Buff;
// Stream buffer type.
typedef boost::asio::streambuf StreamBuff;
// Timer pointer type.
typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;

// Client pointer type.
typedef boost::shared_ptr<IClient> ClientPtr;
// Message pointer type.
typedef boost::shared_ptr<Message> MessagePtr;
// Server pointer type.
typedef boost::shared_ptr<IServer> ServerPtr;
// Session pointer type.
typedef boost::shared_ptr<ISession> SessionPtr;
// Session manager pointer type.
typedef boost::shared_ptr<ISessionMgr> SessionMgrPtr;
// Proxy manager pointer.
typedef boost::shared_ptr<IProxyMgr> ProxyMgrPtr;
// Subscribe manager.
typedef boost::shared_ptr<ISubscribeMgr> SubscribeMgrPtr;

// Session map type.
typedef boost::unordered_map<boost::uuids::uuid, types::SessionPtr> SessionMap;
// List of banned sessions.
typedef std::set<boost::uuids::uuid> BannedList;
// Queue pair type.
typedef std::pair<boost::uuids::uuid, types::MessagePtr> QueuePair;
// Send queue type.
typedef std::queue<QueuePair> SendQueue;
// Connection queue.
typedef boost::unordered_map<boost::uuids::uuid, boost::asio::ip::tcp::endpoint> ConnectQueue;

// Socket pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
// Acceptor pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
// Resolver pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::resolver> ResolverPtr;

// Command pointer type.
typedef boost::shared_ptr<Command> CommandPtr;
// P2P pointer type.
typedef boost::shared_ptr<P2P> P2PPtr;
// Direct pointer type.
typedef boost::shared_ptr<MakeDirect> MakeDirectPtr;

// Error pointer type.
typedef boost::shared_ptr<Errors> ErrorsPtr;
// UPNP client pointer type.
typedef boost::shared_ptr<UPNPClient> UPNPClientPtr;

// Client config.
struct ClientCfg
{
	//! Constructor.
	ClientCfg()
		: isConnected_(false)
	{ }

	//! External endpoint.
	boost::asio::ip::tcp::endpoint extEp_;
	//! Bind endpoint.
	boost::asio::ip::tcp::endpoint bindEp_;
	//! Internal endpoint.
	boost::asio::ip::tcp::endpoint inEp_;
	//! Network mask.
	boost::asio::ip::address_v4 netmask_;
	//! Network.
	boost::asio::ip::address_v4 network_;
	//! Server endpoint.
	boost::asio::ip::tcp::endpoint serverEp_;
	//! Is client connected to the server.
	bool isConnected_;
};
// Client config pointer type.
typedef boost::shared_ptr<ClientCfg> ClientCfgPtr;

// Session type.
enum SessionType
{
	PROXYUP = 0,
	PROXYDOWN,
	DIRECT,
	STUN,
	UNKNOWN
};

// Session config type.
struct SessionConfig
{
	//! Client id.
	boost::uuids::uuid clientId_;
	//! Endpoint list.
	std::queue<boost::asio::ip::tcp::endpoint> epList_;
	//! Session id.
	boost::uuids::uuid sessionId_;
	//! Session type.
	SessionType sessionType_;
	//! Server endpoint.
	boost::asio::ip::tcp::endpoint serverEp_;
};
//
typedef boost::shared_ptr<SessionConfig> SessionConfigPtr;
// Config map type.
typedef boost::unordered_map<boost::uuids::uuid, SessionConfigPtr> ConfigMap;

// Server tuple type.
typedef boost::tuple<boost::uuids::uuid, SessionType, MessagePtr> ServerTuple;
// Server queue type.
typedef std::queue<ServerTuple> ServerQueue;

//! Session pair type.
struct SessionPair
{
	//! Constructor.
	SessionPair(SessionPtr down, SessionPtr up)
		: down_(down)
		, up_(up)
	{ }

	//! Downstream session.
	SessionPtr down_;
	//! Upstream session.
	SessionPtr up_;
};
//! Session pair pointer type.
typedef boost::shared_ptr<SessionPair> PairPtr;
//! Session pair map type.
typedef boost::unordered_map<boost::uuids::uuid, types::PairPtr> ProxyMap;

}

}