#include "pch.h"
#include "proxy/Proxy.h"
#include "proxy/ProxyMgr.h"
#include "proxy/Session.h"
#include "proxy/Utils.h"
#include "proxy/Protocol.h"

namespace proxy
{

Proxy::Proxy(types::ProxyMgrPtr proxyMgr)
	: Core(30)
	, proxyMgr_(proxyMgr)
	, acceptor_(new boost::asio::ip::tcp::acceptor(*ioService_))
{
	Run();
}

Proxy::~Proxy()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "Proxy destruction error: " << e.what();
	}
}

void Proxy::Accept()
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

			auto ep = session->GetSocket()->remote_endpoint();
			LDBG_ << "*** Accepted *** : " << ep.address().to_string() << ":" << ep.port();
			ioStrand_->post(boost::bind(&Proxy::Initialize, this , session));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Accept handler error: " << e.what();
		}

		Accept();
	});
}

void Proxy::Receive(types::SessionPtr from)
{
	boost::asio::async_read_until(
		*from->GetSocket(),
		from->GetBuffRead(),
		protocol::delimiter,
		ioStrand_->wrap([from, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			//LDBG_ << "Receive" << bytes;
			types::PairPtr pair = proxyMgr_->FindPair(from->GetId());
			switch (from->GetType())
			{
			case protocol::DOWNSREAM:
			{
// 				if (pair->up_ && pair->up_->GetReady())
// 				{
					from->SetBytesReadBuff(bytes);
					Send(pair->up_, from);
// 				}
// 				else
// 				{
// 					from->GetBuffRead().consume(bytes);
// 					ioStrand_->post(boost::bind(&Proxy::Receive, this, from));
// 				}
				break;
			}
			case protocol::UPSTREAM:
			{
// 				if (pair->down_ && pair->down_->GetReady())
// 				{
					from->SetBytesReadBuff(bytes);
					Send(pair->down_, from);
// 				}
// 				else
// 				{
// 					from->GetBuffRead().consume(bytes);
// 					ioStrand_->post(boost::bind(&Proxy::Receive, this, from));
// 				}
				break;
			}
			case protocol::UNKNOWN:
			default:
				BOOST_THROW_EXCEPTION(std::runtime_error("Unknown session type"));
			}
		}
		catch (std::exception& e)
		{
			LERR_ << "Receive handler error: " << e.what();
			proxyMgr_->RemoveSession(from->GetId());
		}
	}));
}

void Proxy::Initialize(types::SessionPtr session)
{
	boost::asio::async_read_until(
		*session->GetSocket(),
		session->GetBuffWrite(),
		protocol::delimiter,
		[session, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			LDBG_ << "Proxy initialization request has been received: " << bytes;

			protocol::Event event;
			utils::DeserializeFromBuffer(session->GetBuffWrite(), event, bytes);

			if (event.has_login())
			{
				session->GetBuffWrite().consume(bytes);

				auto login = event.login();
				if (!login.has_session_id() || !login.has_client_id() || !login.has_type())
					BOOST_THROW_EXCEPTION(std::runtime_error("Invalid login request"));

				session->SetClientId(login.client_id());
				session->SetId(login.session_id());
				session->SetType(login.type());

				LDBG_ << "*** Login *** to session with id: "
					<< boost::uuids::to_string(login.session_id());

				proxyMgr_->AddSession(session);

				ioStrand_->post(boost::bind(&Proxy::Initialize, this , session));
			}
			else if (event.has_greeting())
			{
				session->SetBytesWriteBuff(bytes);
				types::PairPtr pair = proxyMgr_->FindPair(session->GetId());
				switch (session->GetType())
				{
				case protocol::DOWNSREAM:
				{
					LDBG_ << "Down *** Greeting *** to session with id: "
						<< boost::uuids::to_string(session->GetId());

					if (pair->up_)
					{
						SendReady(pair->up_, session);
						SendReady(pair->down_, pair->up_);
					}
					break;
				}
				case protocol::UPSTREAM:
				{
					LDBG_ << "Up *** Greeting *** to session with id: "
						<< boost::uuids::to_string(session->GetId());

					if (pair->down_)
					{
						SendReady(pair->down_, session);
						SendReady(pair->up_, pair->down_);
					}
					break;
				}
				case protocol::UNKNOWN:
				default:
					BOOST_THROW_EXCEPTION(std::runtime_error("Unknown session type"));
				}
				ioStrand_->post(boost::bind(&Proxy::Receive, this, session));
			}
			else
			{
				session->GetBuffWrite().consume(bytes);
				BOOST_THROW_EXCEPTION(std::runtime_error("Unknown event type"));
			}
		}
		catch (std::exception& e)
		{
			LERR_ << "Initialization handler error: " << e.what();
			proxyMgr_->RemoveSession(session->GetId());
		}
	});
}

void Proxy::Send(types::SessionPtr to, types::SessionPtr from)
{
	boost::asio::async_write(
		*to->GetSocket(),
		from->GetBuffRead().data(),
		boost::asio::transfer_exactly(from->GetBytesReadBuff()),
		ioStrand_->wrap([to, from, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			//LDBG_ << "Send" << bytes;
			from->GetBuffRead().consume(bytes);
			Receive(from);
		}
		catch (const std::exception& e)
		{
			proxyMgr_->RemoveSession(to->GetId());
			LERR_ << "Send handler error: " << e.what();
		}
	}));
}

void Proxy::SendReady(types::SessionPtr to, types::SessionPtr from)
{
	boost::asio::async_write(
		*to->GetSocket(),
		from->GetBuffWrite().data(),
		boost::asio::transfer_exactly(from->GetBytesWriteBuff()),
		ioStrand_->wrap([to, from, this](const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			LDBG_ << "Proxy ready has been sent: " << bytes;

			from->GetBuffWrite().consume(from->GetBuffWrite().size());
			to->SetReady(true);
		}
		catch (const std::exception& e)
		{
			proxyMgr_->RemoveSession(to->GetId());
			LERR_ << "Ready send handler error: " << e.what();
		}
	}));
}

void Proxy::Start(const unsigned short port)
{
	try
	{
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
		acceptor_->open(endpoint.protocol());
		acceptor_->bind(endpoint);
		acceptor_->listen(boost::asio::socket_base::max_connections);
		Accept();

		LDBG_ << "Proxy has been started on port: " << port;
	}
	catch (const std::exception& e)
	{
		LERR_ << "Unable to start proxy: " << e.what();
	}
}

void Proxy::Stop()
{
	Core::Stop();
}

}