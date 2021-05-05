#pragma once

class NetBIOSScanner
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	NetBIOSScanner();
	//! Destructor.
	~NetBIOSScanner();

	//
	// Public interface.
	//
public:
	//! Starts scanner.
	void Scan();
	//! Resolves hostname to ip address.
	std::string NetBIOSScanner::Resolve(const std::string& hostname);
	//! Resolves ip address to mac address.
	std::string NetBIOSScanner::Arp(const std::string& ipAddress);
};