#include "pch.h"
#include "proxy/Client.h"
#include "proxy/ClientMgr.h"
#include "proxy/Session.h"
#include "proxy/Utils.h"
#include "proxy/Protocol.h"

namespace proxy
{

Client::Client(
	types::ClientMgrPtr clientMgr,
	const protocol::StreamType type,
	const boost::uuids::uuid& id)
	: clientId_(id)
	, clientMgr_(clientMgr)
	, resolver_(new boost::asio::ip::tcp::resolver(*ioService_))
	, type_(type)
{
	Run();
}

Client::~Client()
{
	try
	{
		Disconnect();
		Core::Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "Client destruction error: " << e.what();
	}
}

void Client::Connect(
	const boost::uuids::uuid& sessionId,
	const boost::asio::ip::tcp::endpoint ep)
{
	LDBG_ << "Client is trying to connect to: " << ep.address().to_string() << ":" << ep.port();
	boost::asio::ip::tcp::resolver::query query(
		boost::asio::ip::tcp::v4(),
		ep.address().to_string(),
		boost::lexical_cast<std::string>(ep.port()));

	types::SessionPtr session = Session::Construct(*ioService_);
	session->SetId(sessionId);
	session->SetClientId(clientId_);
	session->SetType(type_);

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

			ioStrand_->post(boost::bind(&Client::DoConnect, this, session, *it));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Unable to resolve given endpoint: " << e.what();
			onError_(session->GetId());
		}
	});
}

void Client::Disconnect()
{
	clientMgr_->Stop();
}

void Client::DoConnect(
	types::SessionPtr session,
	const boost::asio::ip::tcp::endpoint& endpoint)
{
	session->GetSocket()->async_connect(
		endpoint,
		[session, this] (const boost::system::error_code& ec) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			clientMgr_->AddSession(session);
			onConnected_(session->GetId());

			ioStrand_->post(boost::bind(&Client::Login, this, session));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Error during connection to proxy server error: " << e.what();
			onError_(session->GetId());
		}
	});
}

boost::signals2::connection Client::DoOnConnected(const OnConnected::slot_type& signal)
{
	return onConnected_.connect(signal);
}

boost::signals2::connection Client::DoOnError(const OnError::slot_type& signal)
{
	return onError_.connect(signal);
}

boost::signals2::connection Client::DoOnReady(const OnReady::slot_type& signal)
{
	return onReady_.connect(signal);
}

boost::signals2::connection Client::DoOnReceive(const OnReceive::slot_type& signal)
{
	return onReceive_.connect(signal);
}

void Client::DoSend(types::SessionPtr session, protocol::DataPtr data)
{
	boost::asio::async_write(
		*session->GetSocket(),
		boost::asio::buffer(data->data_.data(), data->data_.size()),
		ioStrand_->wrap(
		[this] (const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				LERR_ << "Do send error: " << ec.message();

			boost::mutex::scoped_lock l(mutex_);
			sendQueue_.pop();
			if (!sendQueue_.empty())
			{
				types::SessionPtr next;
				while (!next)
				{
					try
					{
						next = clientMgr_->Find(sendQueue_.front().first);
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

bool Client::IsConnected(const boost::uuids::uuid& sessionId)
{
	try
	{
		if (clientMgr_->Find(sessionId))
			return true;
	}
	catch (const std::exception& e)
	{
		LERR_ << "Session status handler error: " << e.what();
	}
	return false;
}

void Client::Login(types::SessionPtr session)
{
	protocol::Login login;
	login.set_client_id(session->GetClientId());
	login.set_session_id(session->GetId());
	login.set_type(session->GetType());
	protocol::Event event;
	event.set_login(login);

	utils::SerializeToBuffer(event, session->GetBuffWrite());

	boost::asio::async_write(
		*session->GetSocket(),
		session->GetBuffWrite().data(),
		boost::asio::transfer_exactly(session->GetBuffWrite().size()),
		[session, this] (const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			LDBG_ << "Login has been written: " << bytes;
			session->GetBuffWrite().consume(session->GetBuffWrite().size());

			ioStrand_->post(boost::bind(&Client::SendReady, this, session));
		}
		catch (const std::exception& e)
		{
			LERR_ << "Login handler error: " << e.what();
			onError_(session->GetId());
			clientMgr_->RemoveSession(session->GetId());
		}
	});
}

void Client::Receive(types::SessionPtr session)
{
	boost::asio::async_read_until(
		*session->GetSocket(),
		session->GetBuffRead(),
		protocol::delimiter,
		ioStrand_->wrap(
		[session, this] (const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			//LDBG_ << "Receive" << bytes;

			std::stringstream stream;
			utils::BufferToSteam(session->GetBuffRead(), stream, bytes);
			session->GetBuffRead().consume(bytes);

			onReceive_(stream.str(), session->GetId());
			ioStrand_->post(boost::bind(&Client::Receive, this, session));
		}
		catch (std::exception& e)
		{
			LERR_ << "Receiving error: " << e.what();
			onError_(session->GetId());
			clientMgr_->RemoveSession(session->GetId());
		}
	}));
}

void Client::ReceiveReady(types::SessionPtr session)
{
	boost::asio::async_read_until(
		*session->GetSocket(),
		session->GetBuffWrite(),
		protocol::delimiter,
		ioStrand_->wrap(
		[session, this] (const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			LDBG_ << "Ready has been received: " << bytes;

			protocol::Event event;
			utils::DeserializeFromBuffer(session->GetBuffWrite(), event, bytes);
			session->GetBuffWrite().consume(session->GetBuffWrite().size());

			if (!event.has_greeting())
				BOOST_THROW_EXCEPTION(std::runtime_error("Invalid greeting request"));

			auto greeting = event.greeting();
			if (!greeting.has_ready() || !greeting.ready())
				BOOST_THROW_EXCEPTION(std::runtime_error("Opposite client is not ready"));

			session->SetReady(true);
			LDBG_ << "Opposite client is ready";

			ioStrand_->post(boost::bind(&Client::Receive, this, session));
			onReady_(session->GetId());
		}
		catch (std::exception& e)
		{
			LDBG_ << "Ready handler error: " << e.what();
			onError_(session->GetId());
			clientMgr_->RemoveSession(session->GetId());
		}
	}));
}

void Client::Send(const boost::uuids::uuid& sessionId, protocol::DataPtr data)
{
	try
	{
		auto session = clientMgr_->Find(sessionId);
		if (!session->GetReady())
			BOOST_THROW_EXCEPTION(std::runtime_error("Session is not ready yet"));

		{
			boost::mutex::scoped_lock l(mutex_);
			sendQueue_.push(std::make_pair(sessionId, data));
			sendQueue_.back().second->data_.append(protocol::delimiter);

			if (sendQueue_.size() > 1)
				return;
		}

		ioStrand_->post(boost::bind(&Client::DoSend, this, session, data));
	}
	catch (const std::exception& e)
	{
		LERR_ << "Sending error: " << e.what();
	}
}

void Client::SendReady(types::SessionPtr session)
{
	protocol::Greeting greeting;
	greeting.set_ready();
	protocol::Event event;
	event.set_greeting(greeting);
	utils::SerializeToBuffer(event, session->GetBuffWrite());

	boost::asio::async_write(
		*session->GetSocket(),
		session->GetBuffWrite().data(),
		boost::asio::transfer_exactly(session->GetBuffWrite().size()),
		[session, this] (const boost::system::error_code& ec, std::size_t bytes) -> void
	{
		try
		{
			if (ec)
				BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

			LDBG_ << "Ready has been written: " << bytes;
			session->GetBuffWrite().consume(session->GetBuffWrite().size());

			ioStrand_->post(boost::bind(&Client::ReceiveReady, this, session));
		}
		catch (const std::exception& e)
		{
			LDBG_ << "Send ready handler error: " << e.what();
			onError_(session->GetId());
			clientMgr_->RemoveSession(session->GetId());
		}
	});
}

}