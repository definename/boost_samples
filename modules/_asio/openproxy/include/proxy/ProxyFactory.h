#pragma once

#include "proxy/IProxyFactory.h"

namespace proxy
{

class ProxyFactory : public IProxyFactory
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	ProxyFactory();
	//! Destructor.
	~ProxyFactory();

	//
	// Public interface.
	//
public:
	//! Returns client pointer.
	types::ClientPtr ProxyFactory::GetClient(
		const protocol::StreamType type, const boost::uuids::uuid& id);
	//! Returns proxy pointer.
	types::ProxyPtr GetProxy();

	//
	// Private interface.
	//
private:
	//! Returns client manager pointer.
	types::ClientMgrPtr GetClientMgr();
	//! Returns proxy manager pointer.
	types::ProxyMgrPtr GetProxyMgr();

};

}