#pragma once

#include "nat/UPNPCore.h"
#include "nat/Core.h"

namespace opennat
{

class UPNPClient
	: public UPNPCore
	, public Core
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	UPNPClient();
	//! Destructor.
	~UPNPClient();

	//
	// Public types.
	//
public:
	//! Command.
	enum Command
	{
		UPNPCOMMAND_PORTMAPPING_ERROR = 0
	};
	//! Started signal type.
	typedef boost::signals2::signal<void()> OnStarted;


	//
	// Public interface.
	//
public:
	//! Add port mapping.
	unsigned short AddPortMapping(const unsigned short inPort);
	//! Deletes port mapping.
	bool DeletePortMapping(const std::string& extPort);
	//! Sets OnStarted signal.
	boost::signals2::connection DoOnStarted(const OnStarted::slot_type& signal);
	//! Returns external IP address.
	std::string GetExternalIP() const;
	//! Returns local IP address.
	std::string GetLocalIP() const;
	//! Is started.
	bool IsStarted() const;
	//! Starts.
	void Start();
	//! Stops.
	void Stop();

	//
	// Private interface.
	//
private:
	//! Lists redirections.
	void GetPortMappingEntries(const char* url, const char* service, Redirections& redirects);
	//! Is external port mapped to local ip:port.
	bool IsMapped(
		const char* url,
		const char* service,
		const std::string& extPort,
		const std::string& inPort,
		const std::string& inAddr);
	//! Do add port mapping.
	bool DoAddPortMapping(
		const char* url,
		const char* service,
		const std::string& extPort,
		const std::string& inPort,
		const std::string& inAddr,
		const std::string& desc);

	//
	// Private data members.
	//
private:
	//! Synchronization mutex.
	boost::mutex mutex_;
	//! Port mapping history.
	std::set<unsigned short> upnpHistory_;
	//! On started signal.
	OnStarted onStarted_;
	//! Is started.
	bool isStarted_;
};

}