#include "pch.h"
#include "Resolver.h"

Resolver::Resolver()
	: resolver_(ioService_)
{
	Run();
}

Resolver::~Resolver()
{
	Stop();
}

void Resolver::HostByAddress(const boost::asio::ip::address& ipAddress)
{
	boost::asio::ip::address_v4 ip = ipAddress.to_v4();
	boost::asio::ip::tcp::endpoint ep;
	ep.address(ip);

	resolver_.async_resolve(ep, boost::bind(
		&Resolver::HandleResolveByAddress,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));
}

void Resolver::HostByName(const _tstring& hostName)
{
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), hostName, "");
	resolver_.async_resolve(query, boost::bind(
		&Resolver::HandleResolveByName,
		this,
		boost::asio::placeholders::error,
		boost::asio::placeholders::iterator));
}

void Resolver::HandleResolveByAddress(
	const boost::system::error_code& err,
	boost::asio::ip::tcp::resolver::iterator it)
{
	if (err)
	{
		_tcerr << _T("Address resolution error: ") << err.message() << std::endl;
	}
	else
	{
		for (it; it != boost::asio::ip::tcp::resolver::iterator(); ++it)
		{
			_tcout << _T("Address: ") << it->endpoint().address().to_string()
				<< _T(" has been resolved to hostname: ") << it->host_name()
				<< std::endl;
		}
	}
}

void Resolver::HandleResolveByName(
	const boost::system::error_code& err,
	boost::asio::ip::tcp::resolver::iterator it)
{
	if (err)
	{
		_tcerr << _T("Host name resolution error: ") << err.message() << std::endl;
	}
	else
	{
		for (it; it != boost::asio::ip::tcp::resolver::iterator(); ++it)
		{
			_tcout << _T("Hostname: ") << it->host_name()
				<< _T(" has been resolved to address: ")
				<< it->endpoint().address().to_string()
				<< std::endl;
		}
	}
}

void Resolver::Resolve(const _tstring& address)
{
	try
	{
		HostByAddress(boost::asio::ip::address::from_string(address));
	}
	catch (const std::exception&)
	{
		try
		{
			HostByName(address);
		}
		catch (const std::exception& e)
		{
			_tcerr << _T("Error during resolution address: ") << e.what() << std::endl;
		}
	}
}