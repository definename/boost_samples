#pragma once

class ProxySession
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	ProxySession(boost::asio::io_service& ioService);
	//! Destructor.
	~ProxySession();

	//
	// Public types.
	//
public:
	//! Socket type.
	typedef boost::asio::ip::tcp::socket Socket;
	//! Buffer size.
	enum { buffSize = 4096 };

	//
	// Public interface.
	//
public:
	//! Returns buffer.
	char* GetBuffer();
	//! Returns socket.
	Socket& GetSocket();
	//! Gets transferred.
	size_t GetTransferred();
	//! Returns connection state.
	bool IsConnected();
	//! Returns connecting state.
	bool IsConnecting();
	//! Stops session.
	void Stop();
	//! Sets connection state.
	void SetConnected(bool isConnected);
	//! Sets connecting state.
	void SetConnecting(bool isConnecting);
	//! Sets transferred.
	void SetTransferred(const size_t& transferred);

public:
	//! Session input queue.
	std::queue<std::string> inputQueue_;

	//
	// Private data members.
	//
private:
	//! Session socket.
	Socket socket_;
	//! Session buffer.
	char buffer_[buffSize];
	//! Bytes transferred during last read operation.
	size_t transferred_;
	//! Is connected.
	bool isConnected_;
	//! Is session connecting.
	bool isConnecting_;
};