#pragma once

#include "Core.h"

namespace ipc
{

class IPCClient : public Core
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	IPCClient::IPCClient(const std::string& name);
	//! Destructor.
	~IPCClient();

	//
	// Public types.
	//
public:
	//! Queue types.
	enum Types
	{
		MSG_NUM = 100,
		MSG_SIZE = 1024
	};
	//! Receive signal type.
	typedef boost::signals2::signal<void(const std::string& data)> OnReceive;

	//
	// Public interface.
	//
public:
	//! Bind.
	void Create();
	//! Connect.
	void Open();
	//! Sends data.
	void Send(const std::string& data);
	//! Sets OnReceive signal.
	boost::signals2::connection DoOnReceive(const OnReceive::slot_type& signal);
	//! Handles timeout.
	void HandleTimeout(const boost::system::error_code& ec);

	//
	// Private interface.
	//
private:
	//! Receives messages.
	void DoReceive();

	//
	// Private data members.
	//
private:
	//! Shared memory name.
	const std::string name_;
	//! Message queue pointer.
	boost::shared_ptr<boost::interprocess::message_queue> mq_;
	//! Receive signal.
	OnReceive onReceive_;
	//! Open timer.
	boost::shared_ptr<boost::asio::deadline_timer> timer_;
};

}