#include "pch.h"
#include "ProxyClient.h"
#include "ProxySession.h"

ProxyClient::ProxyClient(
	const unsigned short& acceptorPort,
	const std::string& serverIp,
	const unsigned short& serverPort)
	: acceptorPort_(acceptorPort)
	, serverIp_(serverIp)
	, serverPort_(serverPort)
	, sessions_(SessionPair())
{
	std::cout << "Acceptor port: " << acceptorPort_ << std::endl;
	std::cout << "Server ip: " << serverIp_ << std::endl;
	std::cout << "Server port: " << serverPort_ << std::endl;

	ioWork_.reset(new boost::asio::io_service::work(ioService_));

	acceptor_.reset(new boost::asio::ip::tcp::acceptor(
		ioService_, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), acceptorPort)));

	Accept();
	Connect();
}

ProxyClient::~ProxyClient()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}

void ProxyClient::Accept()
{
	try
	{
		SessionPtr& acceptorSession = sessions_.first;
		acceptorSession.reset(new ProxySession(ioService_));

		acceptor_->async_accept(
			acceptorSession->GetSocket(),
			boost::bind(
			&ProxyClient::HandleAccept, this, acceptorSession, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Acceptor error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::Connect()
{
	try
	{
		SessionPtr& connectorSession = sessions_.second;
		connectorSession.reset(new ProxySession(ioService_));

		boost::asio::ip::tcp::resolver resolver(ioService_);
		boost::asio::ip::tcp::resolver::query query(
			boost::asio::ip::tcp::v4(),
			serverIp_.c_str(),
			boost::lexical_cast<std::string>(serverPort_));

		boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
		boost::asio::ip::tcp::endpoint ep = *it;

		connectorSession->GetSocket().async_connect(
			ep,
			boost::bind(
			&ProxyClient::HandleConnect, this, connectorSession, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Connector error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::Close()
{
	try
	{
		std::cout << "Client error" << std::endl;
		boost::mutex::scoped_lock l(mutex_);
		if (sessions_.first)
		{
			sessions_.first->Stop();
			std::cout << "Acceptor session has been stopped" << std::endl;
		}
		if (sessions_.second)
		{
			sessions_.second->Stop();
			std::cout << "Connector session has been stopped" << std::endl;
		}

	}
	catch (const std::exception& e)
	{
		std::cerr << "Error during closing proxy client: " << e.what() << std::endl;
	}
}

void ProxyClient::HandleConnect(SessionPtr session, const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Connected to proxy server" << std::endl;

		session->GetSocket().set_option(boost::asio::socket_base::linger(false, 0));
		session->GetSocket().set_option(boost::asio::ip::tcp::no_delay(true));

		boost::system::error_code er;
		HandleClientWrite(er);
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleServerConnect error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::HandleServerRead(const boost::system::error_code& er, std::size_t transferred)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		sessions_.second->SetTransferred(transferred);

		std::cout << "Write " << transferred << " bytes to client socket" << std::endl;
		std::cout << std::string(sessions_.second->GetBuffer(), sessions_.second->GetTransferred()) << std::endl;
		boost::asio::async_write(
			sessions_.first->GetSocket(),
			boost::asio::buffer(sessions_.second->GetBuffer(), sessions_.second->GetTransferred()),
			boost::bind(&ProxyClient::HandleClientWrite, this, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleServerRead error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::HandleServerWrite(const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Read data from acceptor socket" << std::endl;
		sessions_.first->GetSocket().async_read_some(
			boost::asio::buffer(sessions_.first->GetBuffer(), ProxySession::buffSize),
			boost::bind(
			&ProxyClient::HandleClientRead,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleServerWrite error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::HandleAccept(SessionPtr session, const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Acceptor session has been accepted" << std::endl;

		session->GetSocket().set_option(boost::asio::socket_base::linger(false, 0));
		session->GetSocket().set_option(boost::asio::ip::tcp::no_delay(true));

		boost::system::error_code ec;
		HandleServerWrite(ec);
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleAccept error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::HandleClientRead(const boost::system::error_code& er, std::size_t transferred)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		sessions_.first->SetTransferred(transferred);

		std::cout << "Write " << transferred << " bytes to server socket" << std::endl;

		std::string s(std::string(sessions_.first->GetBuffer(), sessions_.first->GetTransferred()));
		std::cout << std::string(s.begin(), s.begin() + 10) << std::endl;
		boost::asio::async_write(
			sessions_.second->GetSocket(),
			boost::asio::buffer(sessions_.first->GetBuffer(), sessions_.first->GetTransferred()),
			boost::bind(&ProxyClient::HandleServerWrite, this, boost::asio::placeholders::error));

		std::cout << "Write response" << std::endl;
		char r = 0;
		boost::asio::async_write(
			sessions_.first->GetSocket(),
			boost::asio::buffer(&r, 1),
			boost::bind(&ProxyClient::HandleServerWrite, this, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleClientRead error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyClient::HandleClientWrite(const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Read data from server socket" << std::endl;
		sessions_.second->GetSocket().async_read_some(
			boost::asio::buffer(sessions_.second->GetBuffer(), ProxySession::buffSize),
			boost::bind(
			&ProxyClient::HandleServerRead,
			this,
			boost::asio::placeholders::error,
			boost::asio::placeholders::bytes_transferred));
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleClientWrite error: "<< e.what() << std::endl;
		Close();
	}
}

void ProxyClient::Start()
{
	ioService_.run();
}

void ProxyClient::Stop()
{
	Close();

	if (ioWork_)
		ioWork_.reset();

	ioService_.stop();
}