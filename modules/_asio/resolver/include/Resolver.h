#pragma once

#include "Runnable.h"

class Resolver : public Runnable
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Resolver();
	//! Destructor.
	~Resolver();

	//
	// Public interface.
	//
public:
	//! Resolves host name by given ip address.
	void HostByAddress(const boost::asio::ip::address& ipAddress);
	//! Resolves ip address by given host name.
	void HostByName(const _tstring& hostName);
	//! Resolves.
	void Resolve(const _tstring& address);

	//
	// Private interface.
	//
private:
	//! Handles resolve.
	void HandleResolveByName(
		const boost::system::error_code& err,
		boost::asio::ip::tcp::resolver::iterator endpointIterator);
	//! Handles resolve.
	void HandleResolveByAddress(
		const boost::system::error_code& err,
		boost::asio::ip::tcp::resolver::iterator endpointIterator);

	//
	// Private interface.
	//
private:
	//! Resolver.
	boost::asio::ip::tcp::resolver resolver_;
};