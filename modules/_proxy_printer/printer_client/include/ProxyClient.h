#pragma once

/*
*                   +-----------+               +-----------+
* Proxy server  <---|  ip:port  |<--------------|  ip:port  |<---
*               --->| CONNECTOR |-------------->|  ACCEPTOR |--->
*                   +-----------+               +-----------+
*/

//! Forward declaration.
class ProxySession;

class ProxyClient
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	ProxyClient(
		const unsigned short& acceptorPort,
		const std::string& serverIp,
		const unsigned short& serverPort);
	//! Destructor.
	~ProxyClient();

	//
	// Public types.
	//
public:
	//! Proxy session ptr.
	typedef boost::shared_ptr<ProxySession> SessionPtr;
	//! Session pair type.
	typedef std::pair<
		SessionPtr /* Acceptor session */,
		SessionPtr /* Connector session */> SessionPair;
	//! Acceptor smart pointer.
	typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

	//
	// Public interface.
	//
public:
	//! Closes proxy.
	void Close();
	//! Starts proxy.
	void Start();
	//! Stops proxy.
	void Stop();

	//
	// Private interface.
	//
private:
	//! Accepts connection.
	void Accept();
	//! Connects.
	void Connect();

	//! Connects.
	void HandleConnect(SessionPtr session, const boost::system::error_code& er);
	//! Handles reading from the server.
	void HandleServerRead(const boost::system::error_code& er, std::size_t transferred);
	//! Handles writing to the server.
	void HandleServerWrite(const boost::system::error_code& er);

	//! Accepts.
	void HandleAccept(SessionPtr session, const boost::system::error_code& er);
	//! Handles writing to the client.
	void HandleClientRead(const boost::system::error_code& er, std::size_t transferred);
	//! Handles reading from the client.
	void HandleClientWrite(const boost::system::error_code& er);


	//
	// Private data members.
	//
private:
	//! Acceptor.
	AcceptorPtr acceptor_;
	//! Acceptor ip address.
	std::string acceptorIp_;
	//! Acceptor port.
	unsigned short acceptorPort_;
	//! IO service.
	boost::asio::io_service ioService_;
	//! IO inform object.
	std::unique_ptr<boost::asio::io_service::work> ioWork_;
	//! Synchronization mutex.
	mutable boost::mutex mutex_;
	//! Couple of sessions.
	SessionPair sessions_;
	//! Server ip;
	std::string serverIp_;
	//! Server port.
	unsigned short serverPort_;
};