#include "pch.h"
#include "proxy/Client.h"
#include "proxy/ClientMgr.h"
#include "proxy/Proxy.h"
#include "proxy/ProxyFactory.h"
#include "proxy/ProxyMgr.h"

namespace proxy
{

ProxyFactory::ProxyFactory()
{ }

ProxyFactory::~ProxyFactory()
{ }

types::ClientPtr ProxyFactory::GetClient(
	const protocol::StreamType type, const boost::uuids::uuid& id)
{
	return boost::make_shared<Client>(GetClientMgr(), type, id);
}

types::ClientMgrPtr ProxyFactory::GetClientMgr()
{
	return boost::make_shared<ClientMgr>();
}

types::ProxyPtr ProxyFactory::GetProxy()
{
	return boost::make_shared<Proxy>(GetProxyMgr());
}

types::ProxyMgrPtr ProxyFactory::GetProxyMgr()
{
	return boost::make_shared<ProxyMgr>();
}

}