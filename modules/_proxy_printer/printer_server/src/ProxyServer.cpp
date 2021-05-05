#include "pch.h"
#include "ProxyServer.h"
#include "ProxySession.h"

ProxyServer::ProxyServer(
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
}

ProxyServer::~ProxyServer()
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

void ProxyServer::Accept()
{
	try
	{
		SessionPtr& acceptorSession = sessions_.first;
		acceptorSession.reset(new ProxySession(ioService_));

		acceptor_->async_accept(
			acceptorSession->GetSocket(),
			boost::bind(
			&ProxyServer::HandleAccept, this, acceptorSession, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Acceptor error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyServer::Connect()
{
	try
	{
		SessionPtr& connectorSession = sessions_.second;
		connectorSession.reset(new ProxySession(ioService_));
		connectorSession->SetConnecting(true);

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
			&ProxyServer::HandleConnect, this, connectorSession, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Connector error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyServer::Close()
{
	try
	{
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

void ProxyServer::HandleConnect(SessionPtr session, const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Connected to proxy server" << std::endl;

		session->SetConnecting(false);
		session->SetConnected(true);
		session->GetSocket().set_option(boost::asio::socket_base::linger(false, 0));
		session->GetSocket().set_option(boost::asio::ip::tcp::no_delay(true));

		boost::system::error_code er;
		HandleClientWrite(er);
		HandleAcceptorQueue(er);
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleServerConnect error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyServer::HandleServerRead(const boost::system::error_code& er, std::size_t transferred)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		sessions_.second->SetTransferred(transferred);

		std::cout << "*** Write " << transferred << " bytes to client socket ***" << std::endl;

		boost::asio::async_write(
			sessions_.first->GetSocket(),
			boost::asio::buffer(sessions_.second->GetBuffer(), sessions_.second->GetTransferred()),
			boost::bind(&ProxyServer::HandleClientWrite, this, boost::asio::placeholders::error));
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleServerRead error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyServer::HandleServerWrite(const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Read data from acceptor socket" << std::endl;
		sessions_.first->GetSocket().async_read_some(
			boost::asio::buffer(sessions_.first->GetBuffer(), ProxySession::buffSize),
			boost::bind(
			&ProxyServer::HandleClientRead,
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

void ProxyServer::HandleAccept(SessionPtr session, const boost::system::error_code& er)
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

void ProxyServer::HandleAcceptorQueue(const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		boost::mutex::scoped_lock l(mutex_);
		if (!sessions_.first->inputQueue_.empty())
		{
			SessionPtr acceptorSession = sessions_.first;

			std::string message = acceptorSession->inputQueue_.front();
			std::cout << "*** Write buffer from acceptor queue ***" << message.size() << std::endl;
			boost::asio::async_write(
				sessions_.second->GetSocket(),
				boost::asio::buffer(message.data(), message.size()),
				boost::bind(&ProxyServer::HandleAcceptorQueue, this, boost::asio::placeholders::error));

			sessions_.first->inputQueue_.pop();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleAcceptorQueue error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyServer::HandleClientRead(const boost::system::error_code& er, std::size_t transferred)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		SessionPtr& acceptorSession = sessions_.first;
		acceptorSession->SetTransferred(transferred);

		if (sessions_.second && sessions_.second->IsConnected())
		{
			if (acceptorSession->inputQueue_.empty())
			{
				std::cout << "*** Write " << transferred << " bytes to server socket ***" << std::endl;
				boost::asio::async_write(
					sessions_.second->GetSocket(),
					boost::asio::buffer(acceptorSession->GetBuffer(), acceptorSession->GetTransferred()),
					boost::bind(&ProxyServer::HandleServerWrite, this, boost::asio::placeholders::error));

				return;
			}
			else
			{
				std::cout << "Push acceptor buffer to the stack" << std::endl;
				acceptorSession->inputQueue_.push(
					std::string(acceptorSession->GetBuffer(), acceptorSession->GetTransferred()));
			}
		}
		else if (acceptorSession->IsConnecting())
		{
			std::cout << "Push acceptor buffer to the queue" << std::endl;
			acceptorSession->inputQueue_.push(
				std::string(acceptorSession->GetBuffer(), acceptorSession->GetTransferred()));
		}
		else
		{
			std::cout << "Push acceptor buffer to the queue" << std::endl;
			acceptorSession->inputQueue_.push(
				std::string(acceptorSession->GetBuffer(), acceptorSession->GetTransferred()));
			Connect();
		}

		boost::system::error_code ec;
		HandleServerWrite(ec);
	}
	catch (const std::exception& e)
	{
		std::cerr << "HandleClientRead error: " << e.what() << std::endl;
		Close();
	}
}

void ProxyServer::HandleClientWrite(const boost::system::error_code& er)
{
	try
	{
		if (er)
			BOOST_THROW_EXCEPTION(std::runtime_error(er.message()));

		std::cout << "Read data from server socket" << std::endl;
		sessions_.second->GetSocket().async_read_some(
			boost::asio::buffer(sessions_.second->GetBuffer(), ProxySession::buffSize),
			boost::bind(
			&ProxyServer::HandleServerRead,
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

void ProxyServer::Start()
{
	ioService_.run();
}

void ProxyServer::Stop()
{
	Close();

	if (ioWork_)
		ioWork_.reset();

	ioService_.stop();
}