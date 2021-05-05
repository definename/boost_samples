#include "pch.h"
#include "ICMPScanner.h"

ICMPScanner::ICMPScanner()
	: icmpSocket_(ioService_, asio::ip::icmp::v4())
	, icmpId_(static_cast<unsigned short>(::GetCurrentProcessId()))
{
	icmpSocket_.async_receive(
		asio::null_buffers(),
		0,
		boost::bind(&ICMPScanner::HandleReceive, this, asio::placeholders::error));

	Run();
}

ICMPScanner::~ICMPScanner()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		std::cerr << "ICMPScanner destruction error: " << e.what() << std::endl;
	}
}

void ICMPScanner::HandleReceive(const boost::system::error_code& ec)
{
	try
	{
		if (ec)
			BOOST_THROW_EXCEPTION(boost::system::system_error(ec));

		ipv4_header ipv4Header;
		icmp_header icmpHeader;
		std::string body;

		boost::asio::streambuf icmpBuffer;
		auto bytes_transferred = icmpSocket_.receive(icmpBuffer.prepare(65535));
		icmpBuffer.commit(bytes_transferred);
		std::istream pingStream(&icmpBuffer);
		pingStream >> ipv4Header >> icmpHeader >> body;

		if (icmpHeader.type() == icmp_header::echo_reply && icmpHeader.identifier() == icmpId_)
		{
			auto sessionId = boost::uuids::string_generator()(body);

			boost::mutex::scoped_lock l(mutex_);
			auto res = sessions_.find(sessionId);
			if (res == sessions_.end())
			{
				BOOST_THROW_EXCEPTION(std::runtime_error(
					"Reply from "
					+ ipv4Header.source_address().to_string()
					+ " body did not have a registered session id "
					+ boost::uuids::to_string(sessionId)));
			}

			res->second->timer_.expires_from_now(boost::posix_time::seconds(40));

			std::cout << "HOST HAS BEEN FOUND: " << ipv4Header.source_address().to_string() << std::endl;
// 				auto scanSession = res->second;
// 				auto hostInfo = scanSession->add_host(ipv4_header.source_address().to_string());
// 
// 				auto ret = agentsAddressList_.find(hostInfo->to_string());
// 				if (ret != agentsAddressList_.end())
// 					hostInfo->info->set_is_agent(true);
// 
// 				ioService_.post(boost::bind(&NetScan::DoUpdateHostinfo, this, scanSession, hostInfo));
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "ICMP response error: " << e.what();
	}

	icmpSocket_.async_receive(
		asio::null_buffers(),
		0,
		boost::bind(&ICMPScanner::HandleReceive, this, boost::asio::placeholders::error));
}

void ICMPScanner::HandleSend(
	const boost::system::error_code& ec,
	unsigned long endpoint,
	unsigned long lastEndpoint,
	const boost::uuids::uuid& sessionId)
{
	try
	{
		if (ec)
			BOOST_THROW_EXCEPTION(boost::system::system_error(ec));

		if (lastEndpoint && (lastEndpoint > endpoint))
		{
			++endpoint;
			EchoRequest(endpoint, lastEndpoint, sessionId);
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Echo request error: " << e.what() << std::endl;
	}
}

void ICMPScanner::EchoRequest(
	unsigned long endpoint,
	unsigned long lastEndpoint,
	const boost::uuids::uuid& sessionId)
{
	icmp_header echoRequest;
	echoRequest.type(icmp_header::echo_request);
	echoRequest.identifier(icmpId_);

	{
		//! Restarts timer.
		boost::mutex::scoped_lock l(mutex_);
		auto ret = sessions_.find(sessionId);
		if (ret == sessions_.end())
			BOOST_THROW_EXCEPTION(std::runtime_error(
			"Unable to find session with given id " + boost::uuids::to_string(sessionId)));

		ret->second->timer_.expires_from_now(boost::posix_time::seconds(40));
	}

	std::string body(boost::uuids::to_string(sessionId));
	compute_checksum(echoRequest, body.begin(), body.end());

	boost::shared_ptr<asio::streambuf> pingBuffer(new asio::streambuf);
	std::ostream pingStream(pingBuffer.get());
	pingStream << echoRequest << body;

	//! Todo. Remove.
	boost::asio::ip::icmp::endpoint ep(asio::ip::address_v4(endpoint), 0);
	//std::cout << ep.address().to_string() << std::endl;

	icmpSocket_.async_send_to(
		pingBuffer->data(),
		ep,
		0,
		boost::bind(
		&ICMPScanner::HandleSend,
		this,
		asio::placeholders::error,
		endpoint,
		lastEndpoint,
		sessionId));
}

void ICMPScanner::Scan(const asio::ip::address& network, const asio::ip::address& netmask)
{
	//! Todo. How does it work?
	auto curAddress = network.to_v4().to_ulong();
	auto lastAddress = curAddress | (~netmask.to_v4().to_ulong() & 0xFFFFFFFF);

	std::cout << "Scan range " << asio::ip::address_v4(curAddress) << " - " << asio::ip::address_v4(lastAddress) << std::endl;

	if (curAddress > lastAddress)
		BOOST_THROW_EXCEPTION(std::runtime_error(
		"Final address computed to greater than network address"));

	boost::uuids::uuid sessionId = boost::uuids::random_generator()();

	{
		boost::mutex::scoped_lock l(mutex_);
		auto ret = sessions_.insert(std::make_pair(sessionId, SessionPtr()));
		if (!ret.second)
			BOOST_THROW_EXCEPTION(std::runtime_error("Session with given id already exist"));

		ret.first->second.reset(new Session(ioService_, sessionId));

		ret.first->second->timer_.expires_from_now(boost::posix_time::seconds(40));
		ret.first->second->timer_.async_wait(boost::bind(
			&ICMPScanner::HandleSessionTimeout,
			this,
			boost::asio::placeholders::error,
			sessionId));
	}

	EchoRequest(curAddress, curAddress != lastAddress ? lastAddress : 0, sessionId);
}

void ICMPScanner::HandleSessionTimeout(
	const boost::system::error_code& ec, const boost::uuids::uuid& sessionId)
{
	try
	{
		if (ec && ec.value() != boost::asio::error::operation_aborted)
			BOOST_THROW_EXCEPTION(std::runtime_error(ec.message()));

		boost::mutex::scoped_lock l(mutex_);
		auto ret = sessions_.find(sessionId);
		if (ret == sessions_.end())
			BOOST_THROW_EXCEPTION(std::runtime_error("Unable to find sesison with given id"));

		if (ret->second->timer_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
		{
			std::cout << "Session timer has just expired" << std::endl;
			sessions_.erase(sessionId);
			return;
		}

		ret->second->timer_.async_wait(boost::bind(
			&ICMPScanner::HandleSessionTimeout,
			this,
			boost::asio::placeholders::error,
			sessionId));
	}
	catch (const std::exception& e)
	{
		std::cerr << "Session timeout handling error: " << e.what() << std::endl;
	}
}
