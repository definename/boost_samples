#pragma once

namespace proxy
{

//! Forward declaration.
class IClient;
class IClientMgr;
class IProxy;
class IProxyFactory;
class IProxyMgr;
class ISession;

namespace types
{

//! Client pointer type.
typedef boost::shared_ptr<IClient> ClientPtr;
//! Session manager pointer type.
typedef boost::shared_ptr<IClientMgr> ClientMgrPtr;
//! Proxy pointer type.
typedef boost::shared_ptr<IProxy> ProxyPtr;
//! Proxy pointer type.
typedef boost::shared_ptr<IProxyFactory> ProxyFactoryPtr;
//! Session manager pointer type.
typedef boost::shared_ptr<IProxyMgr> ProxyMgrPtr;
//! Session pointer type.
typedef boost::shared_ptr<ISession> SessionPtr;
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
//! Socket pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;
//! Acceptor pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;
//! Buffer pointer type.
typedef boost::asio::streambuf Buff;
//! Resolver pointer type.
typedef boost::shared_ptr<boost::asio::ip::tcp::resolver> ResolverPtr;

}

namespace protocol
{

const unsigned short proxyPort = 21041;
const std::string delimiter = "@ñhunk";

//! Data type.
struct Data
{
	//! Constructor.
	Data(const std::string& data)
		: data_(data)
	{ }

	//! Data member.
	std::string data_;
};
//! Data pointer type.
typedef boost::shared_ptr<Data> DataPtr;

//! Stream type.
enum StreamType
{
	UPSTREAM = 0,
	DOWNSREAM,
	UNKNOWN
};

}

}