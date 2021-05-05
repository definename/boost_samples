#pragma once

#define MEM_FN(x) boost::bind(&SelfType::x, shared_from_this())
#define MEM_FN1(x, y) boost::bind(&SelfType::x, shared_from_this(), y)
#define MEM_FN2(x, y, z) boost::bind(&SelfType::x, shared_from_this(), y, z)

class EchoClient
	: public boost::enable_shared_from_this<EchoClient>
	, private boost::noncopyable
{
	//
	// Construction and destruction.
	//
private:
	//! Constructor.
	EchoClient(const std::string& msg, boost::asio::io_service& io);
public:
	//! Destructor,
	~EchoClient();

	//
	// Public types.
	//
public:
	//! Self pointer type.
	using Ptr = boost::shared_ptr<EchoClient>;
	//! Error code type.
	using ErrorCode = boost::system::error_code;
	//! Message attributes.
	enum class Msg {
		MaxSize = 1024
	};

	//
	// Public interface.
	//
public:
	//! Starts client.
	static Ptr Start(
		const boost::asio::ip::tcp::endpoint& ep,
		const std::string& msg,
		boost::asio::io_service& io);
	//! Stops client.
	void Stop();
	//! Checks whether client is started.
	bool IsStarted() const;

	//
	// Private data members.
	//
private:
	//! Performs connection.
	void Connect(const boost::asio::ip::tcp::endpoint& ep);

	//! Connect handler.
	void OnConnect(
		const boost::asio::ip::tcp::endpoint& ep,
		const boost::system::error_code& ec);
	//! Handles read completion.
	std::size_t OnCompletion(const boost::system::error_code& ec, std::size_t bytes);
	//! Handles read.
	void OnRead(const boost::system::error_code& ec, std::size_t bytes);
	//! Handles write.
	void OnWrite(const boost::system::error_code& ec, std::size_t bytes);

	//
	// Private types.
	//
private:
	//! Self type.
	using SelfType = EchoClient;

	//
	// Private data members.
	//
private:
	//! Started flag.
	std::atomic<bool> isStarted_;
	//! Message.
	std::string msg_;
	//! Write buffer.
	char wrtBuff_[Msg::MaxSize];
	//! Read buffer.
	char readBuff_[Msg::MaxSize];
	//! Client's socket.
	boost::asio::ip::tcp::socket socket_;
};