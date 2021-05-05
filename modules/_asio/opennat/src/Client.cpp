#include "pch.h"
#include "Command.pb.h"
#include "nat/Client.h"
#include "nat/Session.h"
#include "nat/Message.h"
#include "nat/UPNPClient.h"

namespace opennat
{

Client::Client(const boost::uuids::uuid& id, types::SessionMgrPtr sessionMgr)
	: Core(2)
	, acceptor_(new boost::asio::ip::tcp::acceptor(*ioService_))
	, clientId_(id)
	, config_(new types::ClientCfg())
	, connectionTimer_(new boost::asio::deadline_timer(*ioService_))
	, resolver_(new boost::asio::ip::tcp::resolver(*ioService_))
	, sessionMgr_(sessionMgr)
	, upnp_(new UPNPClient())
{
	Run();
}

Client::~Client()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "Client destruction error: " << e.what();
	}
}

void Client::AsyncConnect(const boost::asio::ip::tcp::endpoint ep, const boost::uuids::uuid& id)
{
	if (!upnp_->IsStarted())
	{
		boost::mutex::scoped_lock l(connectMutex_);
		if (!connectQueue_.emplace(id, ep).second)
			LDBG_ << "Session with given id already in connection queue";
	}
	else
	{
		Connect(ep, id);
	}
}

void Client::Bind(const unsigned short port /*= 0*/)
{
	if (!acceptor_->is_open())
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor_->open(endpoint.protocol());
		acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_->bind(endpoint);
		acceptor_->listen(boost::asio::socket_base::max_connections);
		config_->bindEp_ = acceptor_->local_endpoint();
		Accept();

		LDBG_ << "Client has been bound to port: " << config_->bindEp_.port() << " on network interface";
	}
}

void Client::Accept()
{
	types::SessionPtr session = Session::Construct(acceptor_->get_io_service());
	acceptor_->async_accept(
		*session->GetSocket(),
		[session, this](const boost::system::error_code& ec) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			boost::system::error_code ecode;
			session->GetSocket()->set_option(boost::asio::ip::tcp::no_delay(true), ecode);
			if (ecode)
			{
				LERR_ << "Socket no delay error: " << ecode.message();
				ecode.clear();
			}
			session->GetSocket()->set_option(boost::asio::socket_base::keep_alive(true), ecode);
			if (ecode)
			{
				LERR_ << "Socket keep alive error: " << ecode.message();
				ecode.clear();
			}

			auto ep = session->GetSocket()->remote_endpoint(ecode);
			if (ecode)
			{
				LERR_ << "External endpoint error: " << ecode.message();
				ecode.clear();
			}

			LDBG_ << "*** Accepted *** : " << ep.address().to_string() << ":" << ep.port();
			ioStrand_->post(boost::bind(&Client::Receive, this, session));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Accept handler error: " << e.what();
		}

		Accept();
	});
}

types::CommandPtr Client::OpenNAT()
{
	Bind();

	types::CommandPtr cmd(new Command);
	cmd->set_protocol(opennat::Command_Protocol_P2P);

	auto makeDirect = cmd->mutable_p2p()->mutable_make_direct();

	auto inEp = makeDirect->mutable_internal();
	inEp->set_address(config_->inEp_.address().to_string());
	inEp->set_port(boost::lexical_cast<std::string>(config_->bindEp_.port()));
	inEp->set_network(config_->network_.to_string());
	//inEp->set_port("21045");
	LDBG_ << "In direct endpoint: " << inEp->address() << ":" << inEp->port();

	auto ret = upnp_->AddPortMapping(config_->bindEp_.port());
	if (ret != UPNPClient::UPNPCOMMAND_PORTMAPPING_ERROR)
	{
		auto extEp = makeDirect->mutable_external();
		auto upnpExt = upnp_->GetExternalIP();
		extEp->set_address(upnpExt.empty() ? config_->extEp_.address().to_string() : upnpExt);
		extEp->set_port(boost::lexical_cast<std::string>(ret));
		//extEp->set_port("21045");
		LDBG_ << "Ext direct endpoint: " << extEp->address() << ":" << extEp->port();
	}

	return cmd;
}

void Client::Connect(const boost::asio::ip::tcp::endpoint ep, const boost::uuids::uuid& id)
{
	if (!IsRunning())
	{
		LERR_ << "Client is not running";
		return;
	}

	LDBG_ << "Client is trying to connect to: " << ep.address().to_string() << ":" << ep.port();
	boost::asio::ip::tcp::resolver::query query(
		boost::asio::ip::tcp::v4(),
		ep.address().to_string(),
		boost::lexical_cast<std::string>(ep.port()));

	types::SessionPtr session = Session::Construct(*ioService_);
	session->GetSocket()->open(boost::asio::ip::tcp::v4());
	boost::system::error_code ec;
	session->GetSocket()->set_option(boost::asio::ip::tcp::no_delay(true), ec);
	if (ec)
	{
		LERR_ << "Socket no delay error: " << ec.message();
		ec.clear();
	}
	session->GetSocket()->set_option(boost::asio::socket_base::keep_alive(true), ec);
	if (ec)
	{
		LERR_ << "Socket keep alive error: " << ec.message();
		ec.clear();
	}
	session->SetId(id);
	session->SetType(types::STUN);

	// Start stun timer.
	session->GetTimer()->expires_at(boost::posix_time::pos_infin);
	session->GetTimer()->async_wait(boost::bind(
		&Client::HandleStunTimeout, this, boost::asio::placeholders::error, session->GetId()));

	// Start deadline timer.
	session->GetDeadline()->expires_from_now(boost::posix_time::seconds(Session::DEADLINE));
	session->GetDeadline()->async_wait(boost::bind(
		&Client::HandleDeadlineTimeout, this, boost::asio::placeholders::error, session));

	resolver_->async_resolve(
		query,
		[session, ep, this](
		const boost::system::error_code& ec,
		boost::asio::ip::tcp::resolver::iterator it) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			// Start deadline timer.
			session->GetDeadline()->expires_from_now(boost::posix_time::seconds(Session::DEADLINE));
			session->GetDeadline()->async_wait(boost::bind(
				&Client::HandleDeadlineTimeout, this, boost::asio::placeholders::error, session));

			session->GetSocket()->async_connect(
				*it, [session, ep, this](const boost::system::error_code& ec) -> void
			{
				try
				{
					if (ec)
						BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

					if (!sessionMgr_->AddSession(session))
					{
						LERR_ << "Unable to add session with given id";
						onError_(session->GetId(), session->GetType());
						return;
					}

					// Stop deadline timer.
					boost::system::error_code ecode;
					session->GetDeadline()->cancel(ecode);

					config_->serverEp_ = ep;

					// Starts receiver.
					ioStrand_->post(boost::bind(&Client::Receive, this, session));

					// Initializes stun session.
					types::CommandPtr cmd(new Command);
					cmd->set_protocol(opennat::Command_Protocol_Connect);
					auto connect = cmd->mutable_connect();
					connect->set_protocol(opennat::Connect_Protocol_Request);
					connect->set_type(opennat::Connect_Type_Stun);
					connect->set_client_id(boost::uuids::to_string(session->GetId()));
					Send(session->GetId(), cmd);
				}
				catch (const std::exception& e)
				{
					LERR_ << "Error during connection to server: " << e.what();

					// Stop deadline timer.
					boost::system::error_code ecode;
					session->GetDeadline()->cancel(ecode);

					connectionTimer_->expires_from_now(boost::posix_time::seconds(5));
					connectionTimer_->async_wait(boost::bind(
						&Client::HandleConnectionTimeout,
						this,
						boost::asio::placeholders::error,
						ep,
						session->GetId()));
				}
			});
		}
		catch (const std::exception& e)
		{
			LERR_ << "Unable to resolve given endpoint: " << e.what();

			// Stop deadline timer.
			boost::system::error_code ecode;
			session->GetDeadline()->cancel(ecode);

			onError_(session->GetId(), session->GetType());
		}
	});
}

bool Client::IsConnected() const
{
	return config_->isConnected_;
}

void Client::Direct(const boost::uuids::uuid& clientId, const boost::uuids::uuid& sessionId)
{
	auto cmd = OpenNAT();

	auto p2p = cmd->mutable_p2p();
	p2p->set_protocol(opennat::P2P_Protocol_MakeDirect);
	p2p->set_from(boost::uuids::to_string(GetId()));
	p2p->set_to(boost::uuids::to_string(clientId));

	auto makeDirect = p2p->mutable_make_direct();
	makeDirect->set_session_id(boost::uuids::to_string(sessionId));

	Send(GetId(), cmd);
}

void Client::DoDirect(const types::SessionConfigPtr config)
{
	if (config->epList_.empty())
	{
		LERR_ << "Unable to create direct session: " << boost::uuids::to_string(config->sessionId_);
		ioStrand_->post(boost::bind(&Client::Direct, this, config->clientId_, config->sessionId_));
		sessionMgr_->AddBan(config->sessionId_);
		return;
	}

	auto ep = config->epList_.front();
	boost::asio::ip::tcp::resolver::query query(
		boost::asio::ip::tcp::v4(),
		ep.address().to_string(),
		boost::lexical_cast<std::string>(ep.port()));

	LDBG_ << "Connect direct session to: " << ep.address().to_string() << ":" << ep.port();

	types::SessionPtr session = Session::Construct(*ioService_);
	session->SetId(config->sessionId_);
	session->SetType(config->sessionType_);
	session->GetSocket()->open(boost::asio::ip::tcp::v4());
	boost::system::error_code ec;
	session->GetSocket()->set_option(boost::asio::ip::tcp::no_delay(true), ec);
	if (ec)
	{
		LERR_ << "Socket no delay error: " << ec.message();
		ec.clear();
	}
	session->GetSocket()->set_option(boost::asio::socket_base::keep_alive(true), ec);
	if (ec)
	{
		LERR_ << "Socket keep alive error: " << ec.message();
		ec.clear();
	}

	// Start deadline timer.
	session->GetDeadline()->expires_from_now(boost::posix_time::seconds(Session::DEADLINE));
	session->GetDeadline()->async_wait(boost::bind(
		&Client::HandleDeadlineTimeout, this, boost::asio::placeholders::error, session));

	resolver_->async_resolve(
		query,
		[session, config, this](
		const boost::system::error_code& ec,
		boost::asio::ip::tcp::resolver::iterator it) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			// Start deadline timer.
			session->GetDeadline()->expires_from_now(boost::posix_time::seconds(Session::DEADLINE));
			session->GetDeadline()->async_wait(boost::bind(
				&Client::HandleDeadlineTimeout, this, boost::asio::placeholders::error, session));

			session->GetSocket()->async_connect(
				*it, [session, config, this](const boost::system::error_code& ec) -> void
			{
				try
				{
					if (ec)
						BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));
					if (!sessionMgr_->AddSession(session))
						BOOST_THROW_EXCEPTION(std::runtime_error(
						"Unable to add session with given id"));

					// Stop deadline timer.
					boost::system::error_code ecode;
					session->GetDeadline()->cancel(ecode);

					// Starts receiver.
					ioStrand_->post(boost::bind(&Client::Receive, this, session));

					// Initializes direct session.
					types::CommandPtr cmd(new Command);
					cmd->set_protocol(opennat::Command_Protocol_Connect);
					auto connect = cmd->mutable_connect();
					connect->set_protocol(opennat::Connect_Protocol_Request);
					connect->set_type(opennat::Connect_Type_Direct);
					connect->set_client_id(boost::uuids::to_string(session->GetId()));

					Send(session->GetId(), cmd);
				}
				catch (const std::exception& e)
				{
					LERR_ << "Error during connection direct session: " << e.what();

					// Stop deadline timer.
					boost::system::error_code ecode;
					session->GetDeadline()->cancel(ecode);

					config->epList_.pop();
					ioStrand_->post(boost::bind(&Client::DoDirect, this, config));
				}
			});
		}
		catch (const std::exception& e)
		{
			LERR_ << "Unable to resolve given endpoint: " << e.what();

			// Stop deadline timer.
			boost::system::error_code ecode;
			session->GetDeadline()->cancel(ecode);

			config->epList_.pop();
			ioStrand_->post(boost::bind(&Client::DoDirect, this, config));
		}
	});
}

void Client::DoProxy(types::SessionConfigPtr config)
{
	boost::asio::ip::tcp::resolver::query query(
		boost::asio::ip::tcp::v4(),
		config->serverEp_.address().to_string(),
		boost::lexical_cast<std::string>(config->serverEp_.port()));

	LDBG_ << "Connect proxy session to: " 
		<< config->serverEp_.address().to_string() << ":" << config->serverEp_.port();

	types::SessionPtr session = Session::Construct(*ioService_);
	session->SetId(config->sessionId_);
	session->SetRemoteId(config->clientId_);
	session->SetType(config->sessionType_);
	session->GetSocket()->open(boost::asio::ip::tcp::v4());
	boost::system::error_code ec;
	session->GetSocket()->set_option(boost::asio::ip::tcp::no_delay(true), ec);
	if (ec)
	{
		LERR_ << "Socket no delay error: " << ec.message();
		ec.clear();
	}
	session->GetSocket()->set_option(boost::asio::socket_base::keep_alive(true), ec);
	if (ec)
	{
		LERR_ << "Socket keep alive error: " << ec.message();
		ec.clear();
	}

	// Start deadline timer.
	session->GetDeadline()->expires_from_now(boost::posix_time::seconds(Session::DEADLINE));
	session->GetDeadline()->async_wait(boost::bind(
		&Client::HandleDeadlineTimeout, this, boost::asio::placeholders::error, session));

	resolver_->async_resolve(
			query,
			[session, this](
			const boost::system::error_code& ec,
			boost::asio::ip::tcp::resolver::iterator it) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			// Start deadline timer.
			session->GetDeadline()->expires_from_now(boost::posix_time::seconds(Session::DEADLINE));
			session->GetDeadline()->async_wait(boost::bind(
				&Client::HandleDeadlineTimeout, this, boost::asio::placeholders::error, session));

			session->GetSocket()->async_connect(
				*it, [session, this](const boost::system::error_code& ec) -> void
			{
				try
				{
					if (ec)
						BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));
					if (!sessionMgr_->AddSession(session))
						BOOST_THROW_EXCEPTION(std::runtime_error(
						"Unable to add session with given id"));

					// Stop deadline timer.
					boost::system::error_code ecode;
					session->GetDeadline()->cancel(ecode);

					// Initializes proxy session.
					types::CommandPtr cmd(new Command);
					cmd->set_protocol(opennat::Command_Protocol_Connect);
					auto connect = cmd->mutable_connect();
					connect->set_protocol(opennat::Connect_Protocol_Request);
					
					if (session->GetType() == types::PROXYUP)
						connect->set_type(opennat::Connect_Type_Proxyup);
					else if (session->GetType() == types::PROXYDOWN)
						connect->set_type(opennat::Connect_Type_Proxydown);
					else
						BOOST_THROW_EXCEPTION(std::runtime_error("Unknow proxy session type"));

					connect->set_client_id(boost::uuids::to_string(session->GetId()));
					Send(session->GetId(), cmd);

					// Starts receiver.
					ioStrand_->post(boost::bind(&Client::Receive, this, session));
				}
				catch (const std::exception& e)
				{
					LERR_ << "Error during connection proxy session: " << e.what();

					// Stop deadline timer.
					boost::system::error_code ecode;
					session->GetDeadline()->cancel(ecode);

					// TODO: Send error.
					onError_(session->GetId(), session->GetType());
				}
			});
		}
		catch (const std::exception& e)
		{
			LERR_ << "Unable to resolve given endpoint: " << e.what();

			// Stop deadline timer.
			boost::system::error_code ecode;
			session->GetDeadline()->cancel(ecode);

			// TODO: Send error.
			onError_(session->GetId(), session->GetType());
		}
	});
}

void Client::Disconnect()
{
	sessionMgr_->Stop();
}

boost::signals2::connection Client::DoOnConnected(const OnConnected::slot_type& signal)
{
	return onConnected_.connect(signal);
}

boost::signals2::connection Client::DoOnError(const OnError::slot_type& signal)
{
	return onError_.connect(signal);
}

boost::signals2::connection Client::DoOnReceive(const OnReceive::slot_type& signal)
{
	return onReceive_.connect(signal);
}

boost::signals2::connection Client::DoOnReceiveP2P(const OnReceive::slot_type& signal)
{
	return onReceiveP2P_.connect(signal);
}

boost::signals2::connection Client::DoOnSubscription(const OnSubscription::slot_type& signal)
{
	return onSubscription_.connect(signal);
}

void Client::DoSend(types::SessionPtr session, types::MessagePtr msg)
{
	boost::asio::async_write(
		*session->GetSocket(),
		boost::asio::buffer(msg->GetBuff().data(), msg->GetBuff().size()),
		ioStrand_->wrap(
		[this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				LERR_ << "Do send error: " << ec.message();

			boost::mutex::scoped_lock l(sendMutex_);
			sendQueue_.pop();
			if (!sendQueue_.empty())
			{
				types::SessionPtr next;
				while (!next)
				{
					try
					{
						next = sessionMgr_->Find(sendQueue_.front().first);
					}
					catch (const std::exception& e)
					{
						LERR_ << "Queue handling error: " << e.what();
						sendQueue_.pop();
						if (sendQueue_.empty())
							return;
					}
				}
				ioStrand_->post(boost::bind(&Client::DoSend, this, next, sendQueue_.front().second));
			}
		}
		catch (const std::exception& e)
		{
			LERR_ << "Send handler error: " << e.what();
		}
	}));
}

boost::uuids::uuid Client::GetId() const
{
	return clientId_;
}

types::ClientCfgPtr Client::GetConfig() const
{
	return config_;
}

void Client::HandleConnectionTimeout(
	const boost::system::error_code& ec,
	const boost::asio::ip::tcp::endpoint ep,
	const boost::uuids::uuid& id)
{
	try
	{
		if (ec && ec.value() != boost::asio::error::operation_aborted)
			BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

		if (connectionTimer_->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			LERR_ << "Connection timeout has just expired";
			Connect(ep, id);
		}
	}
	catch (const std::exception& e)
	{
		LERR_ << "Connection timeout handling error: " << e.what();
	}
}

void Client::HandleDeadlineTimeout(
	const boost::system::error_code& ec, types::SessionPtr session)
{
	try
	{
		if (ec && ec.value() != boost::asio::error::operation_aborted)
			BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

		if (session->GetDeadline()->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			LERR_ << "Session deadline timeout has just expired";
			session->Stop();
		}
	}
	catch (const std::exception& e)
	{
		LERR_ << "Deadline timeout handler error: " << e.what();
	}
}

void Client::HandleStunTimeout(
	const boost::system::error_code& ec, const boost::uuids::uuid& sessionId)
{
	try
	{
		if (ec && ec.value() != boost::asio::error::operation_aborted)
			BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

		types::SessionPtr session = sessionMgr_->Find(sessionId);
		if (session->GetTimer()->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			//LDBG_ << "Stun timeout...";
			if (session->GetTimeout() == Session::MAXTIMEOUT)
			{
			 	LERR_ << "Session: "
					<< boost::uuids::to_string(session->GetId())
					<< " timeout has just expired";
				sessionMgr_->RemoveSession(session->GetId());
				return;
			}

			session->GetTimeout() += 1;
			opennat::types::CommandPtr cmd(new opennat::Command());
			cmd->set_protocol(opennat::Command_Protocol_Heartbeat);
			auto heartbeat = cmd->mutable_heartbeat();
			heartbeat->set_ping(true);
			Send(session->GetId(), cmd);

			session->GetTimer()->expires_from_now(boost::posix_time::seconds(5));
		}

		session->GetTimer()->async_wait(boost::bind(
			&Client::HandleStunTimeout, this, boost::asio::placeholders::error, session->GetId()));
	}
	catch (const std::exception& e)
	{
		LERR_ << "Stun timeout handling error: " << e.what();
	}
}

bool Client::Process(types::SessionPtr session, types::CommandPtr cmdIn)
{
	bool more = true;
	switch (cmdIn->protocol())
	{
	case opennat::Command_Protocol_Connect:
	{
		if (cmdIn->has_connect())
		{
			auto connectIn = cmdIn->mutable_connect();
			if (connectIn->protocol() == opennat::Connect_Protocol_Request)
			{
				if (connectIn->type() ==  opennat::Connect_Type_Stun)
				{
					LDBG_ << "STUN session request with id: " << connectIn->client_id();
					session->SetId(boost::uuids::string_generator()(connectIn->client_id()));
					session->SetType(opennat::types::STUN);
					if (!sessionMgr_->AddSession(session))
					{
						LERR_ << "Unable to add STUN session with given id: " << connectIn->client_id();
						session->SetId(boost::uuids::nil_uuid());
						session->Stop();

						return more;
					}

					types::CommandPtr cmdOut(new Command);
					cmdOut->set_protocol(opennat::Command_Protocol_Connect);
					auto connectOut = cmdOut->mutable_connect();
					connectOut->set_protocol(opennat::Connect_Protocol_Reply);
					connectOut->set_type(opennat::Connect_Type_Stun);
					connectOut->set_ready(true);

					config_->isConnected_ = true;
					session->SetReady(true);
					onConnected_(session->GetId(), session->GetType());

					Send(session->GetId(), cmdOut);
				}
				else if (connectIn->type() == opennat::Connect_Type_Direct)
				{
					LDBG_ << "DIRECT session request with id: " << connectIn->client_id();
					session->SetId(boost::uuids::string_generator()(connectIn->client_id()));
					session->SetType(types::DIRECT);
					if (!sessionMgr_->AddSession(session))
					{
						LERR_ << "Unable to add DIRECT session with given id: " << connectIn->client_id();
						session->SetId(boost::uuids::nil_uuid());
						session->Stop();

						return more;
					}

					types::CommandPtr cmdOut(new Command);
					cmdOut->set_protocol(opennat::Command_Protocol_Connect);
					auto connectOut = cmdOut->mutable_connect();
					connectOut->set_protocol(opennat::Connect_Protocol_Reply);
					connectOut->set_type(opennat::Connect_Type_Direct);
					connectOut->set_ready(true);
					Send(session->GetId(), cmdOut);

					session->SetReady(true);
					onConnected_(session->GetId(), session->GetType());

					more = false;
				}
				else
					LERR_ << "Invalid connection type";
			}
			else if (connectIn->protocol() == opennat::Connect_Protocol_Reply)
			{
				if (connectIn->type() == opennat::Connect_Type_Stun)
				{
					if (connectIn->has_external())
					{
						auto ext = connectIn->mutable_external();
						config_->extEp_.address(boost::asio::ip::address().from_string(
							ext->address()));
						config_->extEp_.port(boost::lexical_cast<unsigned short>(ext->port()));

						boost::system::error_code ec;
						config_->inEp_ = session->GetSocket()->local_endpoint(ec);
						if (ec)
						{
							LERR_ << "Internal endpoint error: " << ec.message();
							ec.clear();
						}

						config_->netmask_ = boost::asio::ip::address_v4::netmask(
							config_->inEp_.address().to_v4());
						config_->network_ = boost::asio::ip::address_v4(
							config_->inEp_.address().to_v4().to_ulong() &
							config_->netmask_.to_ulong());

						LDBG_ << "Client ext address: " << config_->extEp_.address().to_string();
						LDBG_ << "Client in address: " << config_->inEp_.address().to_string();
						LDBG_ << "Client netmask: " << config_->netmask_.to_string();
						LDBG_ << "Client network: " << config_->network_.to_string();
					}

					if (connectIn->ready())
					{
						LDBG_ << "STUN session: " << boost::uuids::to_string(session->GetId())
							<< " has been connected";
						config_->isConnected_ = true;

						session->SetReady(true);
						// Starts keep_alive timer.
						session->GetTimer()->expires_from_now(boost::posix_time::seconds(5));
						onConnected_(session->GetId(), session->GetType());
					}
				}
				else if (connectIn->type() == opennat::Connect_Type_Direct)
				{
					if (connectIn->ready())
					{
						LDBG_ << "DIRECT session: " << boost::uuids::to_string(session->GetId())
							<< " has been connected";

						session->SetReady(true);
						onConnected_(session->GetId(), session->GetType());

						more = false;
					}
				}
				else if (connectIn->type() == opennat::Connect_Type_Proxyup)
				{
					if (connectIn->initialized())
					{
						LDBG_ << "PROXYUP session: " << session->GetId() << " has been initialized";
						types::CommandPtr cmdOut(new Command());
						cmdOut->set_protocol(opennat::Command_Protocol_P2P);

						auto p2pOut = cmdOut->mutable_p2p();
						p2pOut->set_protocol(opennat::P2P_Protocol_MakeProxy);
						p2pOut->set_to(boost::uuids::to_string(session->GetRemoteId()));
						p2pOut->set_from(boost::uuids::to_string(GetId()));

						auto makeProxy = p2pOut->mutable_make_proxy();
						makeProxy->set_session_id(boost::uuids::to_string(session->GetId()));

						Send(GetId(), cmdOut);
					}
					else if (connectIn->ready())
					{
						LDBG_ << "PROXYUP session: " << session->GetId() << " has been connected";
						// Sends ready signal to downstream session.
						types::CommandPtr cmdOut(new Command());
						cmdOut->set_protocol(opennat::Command_Protocol_Connect);

						auto connectOut = cmdOut->mutable_connect();
						connectOut->set_protocol(opennat::Connect_Protocol_Reply);
						connectOut->set_type(opennat::Connect_Type_Proxydown);
						connectOut->set_ready(true);

						Send(session->GetId(), cmdOut);

						session->SetReady(true);
						onConnected_(session->GetId(), session->GetType());

						more = false;
					}
				}
				else if (connectIn->type() == opennat::Connect_Type_Proxydown)
				{
					if (connectIn->initialized())
					{
						LDBG_ << "PROXYDOWN session: " << session->GetId() << " has been initialized";
						// Sends ready signal to upstream session.
						types::CommandPtr cmdOut(new Command());
						cmdOut->set_protocol(opennat::Command_Protocol_Connect);

						auto connectOut = cmdOut->mutable_connect();
						connectOut->set_protocol(opennat::Connect_Protocol_Reply);
						connectOut->set_type(opennat::Connect_Type_Proxyup);
						connectOut->set_ready(true);

						Send(session->GetId(), cmdOut);
					}
					else if (connectIn->ready())
					{
						LDBG_ << "PROXYDOWN session: " << session->GetId() << " has been connected";
						session->SetReady(true);
						onConnected_(session->GetId(), session->GetType());

						more = false;
					}
				}
				else
					LERR_ << "Invalid connection type";
			}
			else
				LERR_ << "Invalid connection protocol";
		}
		break;
	}
	case opennat::Command_Protocol_Heartbeat:
	{
		auto heartbeat = cmdIn->mutable_heartbeat();
		if (heartbeat->pong())
		{
			//LDBG_ << "Pong from session: " << boost::uuids::to_string(session->GetId());
			session->GetTimeout() = 0;
		}
		else if (heartbeat->ping())
		{
			//LDBG_ << "Ping from session: " << boost::uuids::to_string(session->GetId());
			session->GetTimer()->expires_from_now(boost::posix_time::seconds(10));
			session->GetTimeout() = 0;

			types::CommandPtr cmdOut(new opennat::Command());
			cmdOut->set_protocol(opennat::Command_Protocol_Heartbeat);
			auto heartbeatOut = cmdOut->mutable_heartbeat();
			heartbeatOut->set_pong(true);

			Send(session->GetId(), cmdOut);
		}
		else
			LERR_ << "Invalid heartbeat request";

		break;
	}
	case opennat::Command_Protocol_Subscribe:
	{
		if (cmdIn->has_subscribe())
		{
			auto subscribe = cmdIn->mutable_subscribe();
			if (subscribe->protocol() == opennat::Subscribe_Protocol_Reply)
			{
				if (!subscribe->has_client_id())
					BOOST_THROW_EXCEPTION(std::runtime_error("Invalid client id"));

				onSubscription_(
					boost::uuids::string_generator()(subscribe->client_id()),
					subscribe->online());
			}
			else
				LERR_ << "Invalid subscribe protocol";
		}
		else
			LERR_ << "Invalid subscribe request";

		break;
	}
	default:
		LERR_ << "Invalid command protocol";
		break;
	}
	return more;
}

void Client::ProcessP2P(types::SessionPtr session, types::P2PPtr p2pIn)
{
	switch (p2pIn->protocol())
	{
	case opennat::P2P_Protocol_Error:
	{
		auto error = p2pIn->mutable_error();
		if (error->protocol() == Error_Protocol_Unreachable)
		{
			LERR_ << error->msg();
			onError_(boost::uuids::string_generator()(p2pIn->from()), session->GetType());
		}
		else
			LERR_ << "Invalid error handler";

		break;
	}
	case opennat::P2P_Protocol_MakeDirect:
	{
		if (p2pIn->has_make_direct())
		{
			auto makeDirect = p2pIn->mutable_make_direct();
			boost::uuids::uuid sessionId = boost::uuids::string_generator()(makeDirect->session_id());
			if (makeDirect->has_error())
			{
				auto error = makeDirect->mutable_error();
				sessionMgr_->RemoveBan(sessionId);
				if (error->protocol() == Error_Protocol_Unreachable)
				{
					LERR_ << "Unable to create direct session because of: " << error->msg();

					types::SessionConfigPtr config(new types::SessionConfig());
					config->sessionId_ = sessionId;
					config->sessionType_ = types::PROXYUP;
					config->clientId_ = boost::uuids::string_generator()(p2pIn->from());

					config->serverEp_ = config_->serverEp_;
					DoProxy(config);
				}
			}
			else if (sessionMgr_->IsBanned(sessionId))
			{
				sessionMgr_->RemoveBan(sessionId);

				types::CommandPtr cmd(new Command);
				cmd->set_protocol(opennat::Command_Protocol_P2P);

				auto p2pOut = cmd->mutable_p2p();
				p2pOut->set_protocol(opennat::P2P_Protocol_MakeDirect);
				p2pOut->set_from(boost::uuids::to_string(GetId()));
				p2pOut->set_to(p2pIn->from());

				auto makeDirect = p2pOut->mutable_make_direct();
				makeDirect->set_session_id(boost::uuids::to_string(sessionId));
				auto error = makeDirect->mutable_error();
				error->set_protocol(opennat::Error_Protocol_Unreachable);
				error->set_msg(opennat::GetKernel().GetError()->unreachable_);

				Send(GetId(), cmd);
			}
			else
			{
				types::SessionConfigPtr config(new types::SessionConfig());
				config->sessionId_ = sessionId;
				config->sessionType_ = types::DIRECT;
				config->clientId_ = boost::uuids::string_generator()(p2pIn->from());

				// Internal endpoint.
				if (makeDirect->has_internal())
				{
					auto inEp = makeDirect->mutable_internal();
					if (boost::asio::ip::address_v4::from_string(inEp->network()) == config_->network_)
					{
						config->epList_.emplace(
							boost::asio::ip::address().from_string(inEp->address()),
							boost::lexical_cast<unsigned short>(inEp->port()));
					}
				}

				// External endpoint.
				if (makeDirect->has_external())
				{
					auto extEp = makeDirect->mutable_external();
					config->epList_.emplace(
						boost::asio::ip::address().from_string(extEp->address()),
						boost::lexical_cast<unsigned short>(extEp->port()));
				}

				DoDirect(config);
			}
		}
		else
			LERR_ << "Invalid direct request";

		break;
	}
	case opennat::P2P_Protocol_MakeProxy:
	{
		if (p2pIn->has_make_proxy())
		{
			auto makeProxy = p2pIn->mutable_make_proxy();

			types::SessionConfigPtr config(new types::SessionConfig());
			config->sessionId_ = boost::uuids::string_generator()(makeProxy->session_id());
			config->sessionType_ = types::PROXYDOWN;
			config->clientId_ = boost::uuids::string_generator()(p2pIn->from());
			config->serverEp_ = config_->serverEp_;

			DoProxy(config);
		}
		else
			LERR_ << "Invalid proxy request";

		break;
	}
	case opennat::P2P_Protocol_Data:
	{
		if (p2pIn->has_data())
		{
			onReceiveP2P_(
				boost::uuids::string_generator()(p2pIn->from()),
				reinterpret_cast<const unsigned char*>(p2pIn->data().c_str()),
				p2pIn->data().size());
		}
		else
			LERR_ << "Invalid data request";

		break;
	}
	default:
		LERR_ << "Unknown p2p protocol";
	}
}

void Client::Proxy(types::SessionConfigPtr config)
{
	config->serverEp_ = config_->serverEp_;
	DoProxy(config);
}

void Client::Receive(types::SessionPtr session)
{
	boost::asio::async_read_until(
		*session->GetSocket(),
		session->GetStream(),
		opennat::Protocol().default_instance().delimiter(),
		ioStrand_->wrap(
		[session, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			types::CommandPtr cmd(new Command);
			if (!cmd->ParseFromArray(
				boost::asio::buffer_cast<const unsigned char*>(session->GetStream().data()),
				static_cast<int>(bytes - opennat::Protocol().default_instance().delimiter().size())))
				BOOST_THROW_EXCEPTION(std::runtime_error("Unable to parse protocol buffer"));

			if (cmd->protocol() == opennat::Command_Protocol_P2P && cmd->has_p2p())
			{
				ProcessP2P(session, boost::make_shared<opennat::P2P>(cmd->p2p()));

				session->GetStream().consume(bytes);
				ioStrand_->post(boost::bind(&Client::Receive, this, session));
			}
			else
			{
				if (Process(session, cmd))
				{
					session->GetStream().consume(bytes);
					ioStrand_->post(boost::bind(&Client::Receive, this, session));
				}
				else
				{
					session->GetStream().consume(bytes);
					ioStrand_->post(boost::bind(&Client::ReceiveRaw, this, session));
				}
			}
		}
		catch (const std::exception& e)
		{
			LERR_ << "Receiving error: " << e.what();
			if (session->GetId() == GetId())
				config_->isConnected_ = false;

			sessionMgr_->RemoveSession(session->GetId());
			onError_(session->GetId(), session->GetType());
		}
	}));
}

void Client::ReceiveRaw(types::SessionPtr session)
{
	boost::asio::async_read_until(
		*session->GetSocket(),
		session->GetStream(),
		opennat::Protocol().default_instance().delimiter(),
		ioStrand_->wrap(
		[session, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			onReceive_(
				session->GetId(),
				boost::asio::buffer_cast<const unsigned char*>(session->GetStream().data()),
				static_cast<int>(bytes - opennat::Protocol().default_instance().delimiter().size()));

			session->GetStream().consume(bytes);
			ioStrand_->post(boost::bind(&Client::ReceiveRaw, this, session));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Raw data receiving error: " << e.what();
			if (session->GetId() == GetId())
				config_->isConnected_ = false;

			sessionMgr_->RemoveSession(session->GetId());
			onError_(session->GetId(), session->GetType());
		}
	}));
}

void Client::Send(const boost::uuids::uuid& sessionId, types::CommandPtr cmd)
{
	try
	{
		auto session = sessionMgr_->Find(sessionId);
		opennat::types::MessagePtr msg(new Message);
		msg->SetBuff<decltype(cmd)>(cmd);
		Send(sessionId, msg);
	}
	catch (const std::exception& e)
	{
		LERR_ << "Protocol data transferring error: " << e.what();
	}
}

void Client::Send(const boost::uuids::uuid& sessionId, types::MessagePtr msg)
{
	try
	{
		auto session = sessionMgr_->Find(sessionId);
		msg->GetBuff().insert(
			msg->GetBuff().end(),
			std::begin(opennat::Protocol().default_instance().delimiter()),
			std::end(opennat::Protocol().default_instance().delimiter()));
		{
			boost::mutex::scoped_lock l(sendMutex_);
			sendQueue_.emplace(sessionId, msg);

			if (sendQueue_.size() > 1)
				return;
		}

		ioStrand_->post(boost::bind(&Client::DoSend, this, session, msg));
	}
	catch (const std::exception& e)
	{
		LERR_ << "Custom data transferring error: " << e.what();
	}
}

void Client::Send2Client(const boost::uuids::uuid& clientId, types::MessagePtr msg)
{
	types::CommandPtr cmd(new Command());
	cmd->set_protocol(opennat::Command_Protocol_P2P);

	auto p2p = cmd->mutable_p2p();
	p2p->set_protocol(opennat::P2P_Protocol_Data);
	p2p->set_from(boost::uuids::to_string(GetId()));
	p2p->set_to(boost::uuids::to_string(clientId));
	p2p->set_data(msg->GetBuff().data(), msg->GetBuff().size());

	Send(GetId(), cmd);
}

void Client::Send2Server(types::MessagePtr msg)
{
	types::CommandPtr cmd(new Command());
	cmd->set_protocol(opennat::Command_Protocol_Data);
	cmd->set_data(msg->GetBuff().data(), msg->GetBuff().size());

	Send(GetId(), cmd);
}

void Client::StartUPNP()
{
	upnp_->Start();
	upnp_->DoOnStarted([this]() -> void
	{
		LDBG_ << "UPNP client has been started";
		boost::mutex::scoped_lock l(connectMutex_);
		for (const auto& val : connectQueue_)
		{
			Connect(val.second, val.first);
		}
		connectQueue_.clear();
	});
}


void Client::Stop()
{
	if (!IsRunning())
	{
		LDBG_ << "Client is already stopped";
		return;
	}

	boost::system::error_code ec;
	connectionTimer_->cancel(ec);

	if (upnp_)
		upnp_->Stop();

	Disconnect();
	Core::Stop();

}

void Client::Subscribe(const boost::uuids::uuid& clientId)
{
	opennat::types::CommandPtr cmd(new opennat::Command());
	cmd->set_protocol(opennat::Command_Protocol_Subscribe);

	auto subscribe = cmd->mutable_subscribe();
	subscribe->set_protocol(opennat::Subscribe_Protocol_Request);
	subscribe->set_client_id(boost::uuids::to_string(clientId));

	Send(GetId(), cmd);
}

}