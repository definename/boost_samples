#pragma once

#include "proxy/IProxy.h"
#include "proxy/Core.h"

namespace proxy
{

class Proxy 
	: public IProxy
	, public Core
	, private boost::noncopyable
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	explicit Proxy(types::ProxyMgrPtr proxyMgr);
	//! Destructor.
	~Proxy();

	//
	// Public interface
	//
public:
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
	//! Initializes session.
	void Initialize(types::SessionPtr session);
	//! Receives.
	void Receive(types::SessionPtr from);
	//! Sends.
	void Send(types::SessionPtr to, types::SessionPtr from);
	//! Sends ready request.
	void SendReady(types::SessionPtr to, types::SessionPtr from);

	//
	// Private data members.
	//
private:
	//! Proxy manager pointer.
	types::ProxyMgrPtr proxyMgr_;
	//! Acceptor pointer.
	types::AcceptorPtr acceptor_;
};

}