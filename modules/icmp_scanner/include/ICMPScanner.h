#pragma once

#include "Runnable.h"

class ICMPScanner : public Runnable
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	ICMPScanner();
	//! Destructor.
	~ICMPScanner();

	//
	// Public interface.
	//
public:
	//! Starts scanner.
	void Scan(const asio::ip::address& network, const asio::ip::address& netmask);

	//
	// Public types.
	//
public:
	//! Session.
	class Session
	{
		//
		// Construction and destruction.
		//
	public:
		//! Constructor.
		Session(boost::asio::io_service& ioService, const boost::uuids::uuid& sessionId)
			: timer_(ioService)
			, sessionId_(sessionId)
		{ }
		//! Destructor.
		~Session() { }

		//
		// Public data members.
		//
	public:
		//! Timer.
		boost::asio::deadline_timer timer_;
		//! Session id.
		boost::uuids::uuid sessionId_;
	};
	//! Session smart pointer type.
	typedef boost::shared_ptr<Session> SessionPtr;
	//! Session type.
	typedef std::map<boost::uuids::uuid, SessionPtr> Sessions;

	//
	// Private interface.
	//
private:
	//! Sends echo request.
	void EchoRequest(
		unsigned long endpoint,
		unsigned long lastEndpoint,
		const boost::uuids::uuid& sessionId);
	//! Handles receive.
	void HandleReceive(const boost::system::error_code& ec);
	//! Handles send.
	void HandleSend(
	const boost::system::error_code& ec,
	unsigned long endpoint,
	unsigned long lastEndpoint,
	const boost::uuids::uuid& sessionId);
	//! Handle session timeout.
	void HandleSessionTimeout(
		const boost::system::error_code& ec, const boost::uuids::uuid& sessionId);

	//
	// Private data members.
	//
private:
	//! Sessions container.
	Sessions sessions_;
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! ICMP socket.
	boost::asio::ip::icmp::socket icmpSocket_;
	//! ICMP socket id.
	unsigned short icmpId_;
};