#pragma once

namespace opennat
{


class UPNPCore
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	UPNPCore();
	//! Destructor.
	~UPNPCore();

	//
	// Public types.
	//
protected:
	//! Redirection type.
	typedef struct Entry {
		//! Description.
		std::string desc_;
		//! External port.
		unsigned short extPort_;
		//! Internal port.
		unsigned short inPort_;
		//! Internal address.
		std::string inAddr_;
		//! Protocol.
		std::string protocol_;
		//! Index.
		int index_;
	} Entry;
	//! Redirection list types.
	typedef std::vector<Entry> Redirections;


	//
	// Public interface.
	//
public:
	//! Is started.
	bool IsConnected() const;

	//
	// Private interface.
	//
protected:
	//! Connects.
	void Connect();
	//! Disconnects.
	void Disconnect();

	//
	// Private data members.
	//
protected:
	//! Local address.
	std::array<char, 64> localAddr_;
	//! Control URL.
	std::string controlURL_;
	//! Service type.
	std::string serviceType_;
	//! Is connected.
	bool isConnect_;
};

}