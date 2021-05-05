#pragma once

#include "proxy/Types.h"

namespace proxy
{

class IProxyFactory
{
	//
	// Construction and destruction.
	//
public:
	//! Destructor.
	virtual ~IProxyFactory() { };

	//
	// Public interface.
	//
public:
	//! Returns client pointer.
	virtual types::ClientPtr GetClient(
		const protocol::StreamType type, const boost::uuids::uuid& id) = 0;
	//! Returns proxy pointer.
	virtual types::ProxyPtr GetProxy() = 0;

	//
	// Private interface.
	//
protected:
	//! Returns client manager pointer.
	virtual types::ClientMgrPtr GetClientMgr() = 0;
	//! Returns proxy manager pointer.
	virtual types::ProxyMgrPtr GetProxyMgr() = 0;
};

}