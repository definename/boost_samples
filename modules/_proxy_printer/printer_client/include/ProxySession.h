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
	//! Stops session.
	void Stop();
	//! Sets transferred.
	void SetTransferred(const size_t& transferred);

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
};