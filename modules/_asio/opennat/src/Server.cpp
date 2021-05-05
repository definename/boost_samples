#include "pch.h"
#include "nat/Message.h"
#include "Command.pb.h"
#include "nat/Server.h"
#include "nat/Session.h"
#include "nat/Utils.h"

namespace opennat
{

Server::Server(
	types::SessionMgrPtr sessionMgr,
	types::ProxyMgrPtr proxyMgr,
	types::SubscribeMgrPtr subscribeMgr)
	: Core(30)
	, acceptor_(new boost::asio::ip::tcp::acceptor(*ioService_))
	, proxyMgr_(proxyMgr)
	, stunMgr_(sessionMgr)
	, subscribeMgr_(subscribeMgr)
{
	subscribeMgr_->DoOnNotify(boost::bind(&Server::Send, this, _1, _2, _3));

	Run();
}

Server::~Server()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "Server destruction error: " << e.what();
	}
}

void Server::Accept()
{
	types::SessionPtr session = Session::Construct(acceptor_->get_io_service());
	acceptor_->async_accept(
		*session->GetSocket(),
		[session, this] (const boost::system::error_code& ec) -> void
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
			ioStrand_->post(boost::bind(&Server::Receive, this, session));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Accept handler error: " << e.what();
		}

		Accept();
	});
}

boost::signals2::connection Server::DoOnReceive(const OnReceive::slot_type& signal)
{
	return onReceive_.connect(signal);
}

boost::signals2::connection Server::DoOnDisconnected(const OnDisconnected::slot_type& signal)
{
	return onDisconnected_.connect(signal);
}

void Server::DoSend(types::SessionPtr session, types::MessagePtr msg)
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

			boost::mutex::scoped_lock l(mutex_);
			serverQueue_.pop();
			if (!serverQueue_.empty())
			{
				types::SessionPtr next = nullptr;
				while (!next)
				{
					try
					{
						auto type = serverQueue_.front().get<1>();
						auto sessionId = serverQueue_.front().get<0>();
						if (type == types::STUN)
							next = stunMgr_->Find(sessionId);
						else if (type == types::PROXYUP)
							next = proxyMgr_->FindPair(sessionId)->up_;
						else if (type == types::PROXYDOWN)
							next = proxyMgr_->FindPair(sessionId)->down_;
						else
							BOOST_THROW_EXCEPTION(std::runtime_error("Unknown session type"));
					}
					catch (const std::exception& e)
					{
						LERR_ << "Server queue handling error: " << e.what();
						serverQueue_.pop();
						if (serverQueue_.empty())
							return;
					}
				}
				ioStrand_->post(boost::bind(&Server::DoSend, this, next, serverQueue_.front().get<2>()));
			}
		}
		catch (const std::exception& e)
		{
			LERR_ << "Send handler error: " << e.what();
		}
	}));
}

void Server::HandleStunTimeout(
	const boost::system::error_code& ec, const boost::uuids::uuid& sessionId)
{
	try
	{
		if (ec && ec.value() != boost::asio::error::operation_aborted)
			BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

		types::SessionPtr session = stunMgr_->Find(sessionId);
		if (session->GetTimer()->expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			//LDBG_ << "Stun timeout...";
			if (session->GetTimeout() == Session::MAXTIMEOUT)
			{
				LERR_ << "Session: "
					<< boost::uuids::to_string(session->GetId())
					<< " timeout has just expired";
				stunMgr_->RemoveSession(session->GetId());
				return;
			}

			session->GetTimeout() += 1;
			opennat::types::CommandPtr cmd(new opennat::Command());
			cmd->set_protocol(opennat::Command_Protocol_Heartbeat);
			auto heartbeat = cmd->mutable_heartbeat();
			heartbeat->set_ping(true);
			Send(session->GetId(), types::STUN, cmd);

			session->GetTimer()->expires_from_now(boost::posix_time::seconds(5));
		}

		session->GetTimer()->async_wait(boost::bind(
			&Server::HandleStunTimeout, this, boost::asio::placeholders::error, session->GetId()));
	}
	catch (const std::exception& e)
	{
		LERR_ << "Ping timeout handling error: " << e.what();
	}
}

bool Server::Process(types::SessionPtr session, types::CommandPtr cmdIn)
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
				if (connectIn->type() == opennat::Connect_Type_Stun)
				{
					LDBG_ << "STUN session request with id: " << connectIn->client_id();
					session->SetId(boost::uuids::string_generator()(connectIn->client_id()));
					session->SetType(opennat::types::STUN);
					// Starts keep_alive timer.
					session->GetTimer()->expires_from_now(boost::posix_time::seconds(5));
					session->GetTimer()->async_wait(boost::bind(
						&Server::HandleStunTimeout,
						this,
						boost::asio::placeholders::error,
						session->GetId()));
					if (!stunMgr_->AddSession(session))
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

					auto ext = connectOut->mutable_external();

					boost::system::error_code ec;
					auto ep = session->GetSocket()->remote_endpoint(ec);
					if (ec)
					{
						LERR_ << "External endpoint error: " << ec.message();
						ec.clear();
					}

					ext->set_address(ep.address().to_string());
					ext->set_port(boost::lexical_cast<std::string>(ep.port()));

					subscribeMgr_->Notify(session->GetId(), true);
					Send(session->GetId(), types::STUN, cmdOut);
				}
				else if (connectIn->type() == opennat::Connect_Type_Proxyup)
				{
					LDBG_ << "PROXYUP session request with id: " << connectIn->client_id();
					session->SetId(boost::uuids::string_generator()(connectIn->client_id()));
					session->SetType(types::PROXYUP);
					if (!proxyMgr_->AddSession(session))
					{
						LERR_ << "Unable to add proxy session with given id" << connectIn->client_id();
						session->SetId(boost::uuids::nil_uuid());
						session->Stop();
						return more;
					}

					types::CommandPtr cmdOut(new Command);
					cmdOut->set_protocol(opennat::Command_Protocol_Connect);
					auto connectOut = cmdOut->mutable_connect();
					connectOut->set_protocol(opennat::Connect_Protocol_Reply);
					connectOut->set_type(opennat::Connect_Type_Proxyup);
					connectOut->set_initialized(true);
					Send(session->GetId(), types::PROXYUP, cmdOut);

					more = false;
				}
				else if (connectIn->type() == opennat::Connect_Type_Proxydown)
				{
					LDBG_ << "PROXYDOWN session request with id: " << connectIn->client_id();
					session->SetId(boost::uuids::string_generator()(connectIn->client_id()));
					session->SetType(types::PROXYDOWN);
					if (!proxyMgr_->AddSession(session))
					{
						LERR_ << "Unable to add proxy session with given id" << connectIn->client_id();
						session->SetId(boost::uuids::nil_uuid());
						session->Stop();
						return more;
					}

					types::CommandPtr cmdOut(new Command);
					cmdOut->set_protocol(opennat::Command_Protocol_Connect);
					auto connectOut = cmdOut->mutable_connect();
					connectOut->set_protocol(opennat::Connect_Protocol_Reply);
					connectOut->set_type(opennat::Connect_Type_Proxydown);
					connectOut->set_initialized(true);
					Send(session->GetId(), types::PROXYDOWN, cmdOut);

					more = false;
				}
				else
					LERR_ << "Invalid connection type";
			}
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

			Send(session->GetId(), session->GetType(), cmdOut);
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
			if (subscribe->protocol() == opennat::Subscribe_Protocol_Request)
			{
				if (!subscribe->has_client_id())
					BOOST_THROW_EXCEPTION(std::runtime_error("Invalid client list"));

				subscribeMgr_->Subscribe(
					boost::uuids::string_generator()(subscribe->client_id()),
					session->GetId(),
					stunMgr_->Count(boost::uuids::string_generator()(subscribe->client_id())) > 0);
			}
			else
				LERR_ << "Invalid subscribe protocol";
		}
		else
			LERR_ << "Invalid subscribe request";

		break;
	}
	case opennat::Command_Protocol_Data:
	{
		if (cmdIn->has_data())
		{
			onReceive_(
				session->GetId(),
				reinterpret_cast<const unsigned char*>(cmdIn->data().c_str()),
				cmdIn->data().size());
		}
		else
			LERR_ << "Invalid data request";

		break;
	}
	default:
		LERR_ << "Invalid command protocol";
		break;
	}

	return more;
}

void Server::Receive(const types::SessionPtr session)
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
				session->SetBytesTransferred(bytes);
				SendP2P(session, boost::uuids::string_generator()(cmd->mutable_p2p()->to()));
			}
			else
			{
				if (Process(session, cmd))
				{
					session->GetStream().consume(bytes);
					ioStrand_->post(boost::bind(&Server::Receive, this, session));
				}
				else
				{
					session->GetStream().consume(bytes);
					ioStrand_->post(boost::bind(&Server::ReceiveProxy, this, session));
				}
			}
		}
		catch (const std::exception& e)
		{
			LERR_ << "Receiving error: " << e.what();
			stunMgr_->RemoveSession(session->GetId());
			subscribeMgr_->Notify(session->GetId(), false);
			onDisconnected_(session->GetId());
		}
	}));
}

void Server::ReceiveProxy(const types::SessionPtr from)
{
	boost::asio::async_read_until(
		*from->GetSocket(),
		from->GetStream(),
		opennat::Protocol().default_instance().delimiter(),
		ioStrand_->wrap(
		[from, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			types::PairPtr pair = proxyMgr_->FindPair(from->GetId());
			from->SetBytesTransferred(bytes);
			switch (from->GetType())
			{
			case types::PROXYDOWN:
			{
				SendProxy(pair->down_, pair->up_);
				break;
			}
			case types::PROXYUP:
			{
				SendProxy(pair->up_, pair->down_);
				break;
			}
			default:
				BOOST_THROW_EXCEPTION(std::runtime_error("Invalid session type"));
				break;
			}
		}
		catch (const std::exception& e)
		{
			LERR_ << "Proxy receiver error: " << e.what();
			proxyMgr_->RemoveSession(from->GetId());
		}
	}));
}

void Server::Send(
	const boost::uuids::uuid& sessionId,
	const types::SessionType type,
	types::CommandPtr cmd)
{
	try
	{
		types::SessionPtr session = nullptr;
		if (type == types::STUN)
		{
			session = stunMgr_->Find(sessionId);
			if (!session)
				BOOST_THROW_EXCEPTION(std::runtime_error("Invalid stun session pointer"));
		}
		else if (type == types::PROXYUP)
		{
			session = proxyMgr_->FindPair(sessionId)->up_;
			if (!session)
				BOOST_THROW_EXCEPTION(std::runtime_error("Invalid upstream session pointer"));
		}
		else if (type == types::PROXYDOWN)
		{
			session = proxyMgr_->FindPair(sessionId)->down_;
			if (!session)
				BOOST_THROW_EXCEPTION(std::runtime_error("Invalid downstream session pointer"));
		}
		else
			BOOST_THROW_EXCEPTION(std::runtime_error("Unknown session type"));

		types::MessagePtr msg(new Message);
		msg->SetBuff<decltype(cmd)>(cmd);
		msg->GetBuff().insert(
			msg->GetBuff().end(),
			std::begin(opennat::Protocol().default_instance().delimiter()),
			std::end(opennat::Protocol().default_instance().delimiter()));

		{
			boost::mutex::scoped_lock l(mutex_);
			serverQueue_.emplace(sessionId, type, msg);

			if (serverQueue_.size() > 1)
				return;
		}

		ioStrand_->post(boost::bind(&Server::DoSend, this, session, msg));
	}
	catch (const std::exception& e)
	{
		LERR_ << "Sending error: " << e.what();
	}
}

void Server::SendProxy(const types::SessionPtr from, const types::SessionPtr to)
{
	boost::asio::async_write(
		*to->GetSocket(),
		boost::asio::buffer(
		boost::asio::buffer_cast<const unsigned char*>(from->GetStream().data()),
		from->GetBytesTransferred()),
		ioStrand_->wrap(
		[from, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			from->GetStream().consume(bytes);
			ioStrand_->post(boost::bind(&Server::ReceiveProxy, this, from));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Error during proxing data: " << e.what();
		}
	}));
}


void Server::SendP2P(const types::SessionPtr from, const boost::uuids::uuid clientId)
{
	try
	{
		auto to = stunMgr_->Find(clientId);
		boost::asio::async_write(
			*to->GetSocket(),
			boost::asio::buffer(
			boost::asio::buffer_cast<const unsigned char*>(from->GetStream().data()),
			from->GetBytesTransferred()),
			ioStrand_->wrap(
			[to, from, this](const boost::system::error_code& ec, std::size_t bytes) -> void
		{
			try
			{
				if (ec)
					BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));
			}
			catch (const std::exception& e)
			{
				LERR_ << "P2P error: " << e.what();

				// Sends p2p error.
				types::CommandPtr cmd(new Command);
				cmd->set_protocol(opennat::Command_Protocol_P2P);

				auto p2p = cmd->mutable_p2p();
				p2p->set_protocol(opennat::P2P_Protocol_Error);
				p2p->set_from(boost::uuids::to_string(to->GetId()));
				p2p->set_to(boost::uuids::to_string(from->GetId()));

				auto error = p2p->mutable_error();
				error->set_msg(opennat::GetKernel().GetError()->unreachable_);
				error->set_protocol(opennat::Error_Protocol_Unreachable);

				Send(boost::uuids::string_generator()(p2p->to()), types::STUN, cmd);
			}

// 			// Starts receiver.
// 			from->GetStream().consume(bytes);
// 			ioStrand_->post(boost::bind(&Server::Receive, this, from));
		}));

	}
	catch (const std::exception& e)
	{
		LERR_ << "P2P error: " << e.what();

		// Sends p2p error.
		types::CommandPtr cmd(new Command);
		cmd->set_protocol(opennat::Command_Protocol_P2P);

		auto p2p = cmd->mutable_p2p();
		p2p->set_protocol(opennat::P2P_Protocol_Error);
		p2p->set_from(boost::uuids::to_string(clientId));
		p2p->set_to(boost::uuids::to_string(from->GetId()));

		auto error = p2p->mutable_error();
		error->set_msg(opennat::GetKernel().GetError()->unreachable_);
		error->set_protocol(opennat::Error_Protocol_Unreachable);

		Send(from->GetId(), types::STUN, cmd);
	}

	// Starts receiver.
	from->GetStream().consume(from->GetBytesTransferred());
	ioStrand_->post(boost::bind(&Server::Receive, this, from));
}

void Server::Start(const unsigned short port)
{
	try
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor_->open(endpoint.protocol());
		acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		acceptor_->bind(endpoint);
		acceptor_->listen(boost::asio::socket_base::max_connections);
		Accept();

		LDBG_ << "Opennat server has been started on port: " << port;
	}
	catch (const std::exception& e)
	{
		LERR_ << "Unable to start opennat server: " << e.what();
	}
}

void Server::Stop()
{
	stunMgr_->Stop();
	Core::Stop();
}

}