#include "pch.h"
#include "NetBIOSScanner.h"

NetBIOSScanner::NetBIOSScanner()
{ }

NetBIOSScanner::~NetBIOSScanner()
{ }

void NetBIOSScanner::Scan()
{
	SERVER_INFO_101 *sInfo = nullptr;
	SERVER_INFO_101 *sFirst = nullptr;
	DWORD entriesRead = 0;
	DWORD totalEntries = 0;
	DWORD resumeHandle = 0;

	std::cout << "Start scanning network via NetServerEnum" << std::endl;
	auto status = ::NetServerEnum(
		NULL,
		101,
		(LPBYTE*)&sInfo,
		MAX_PREFERRED_LENGTH,
		&entriesRead,
		&totalEntries,
		SV_TYPE_ALL,
		NULL,
		&resumeHandle);

	if ((status != NERR_Success) && (status != ERROR_MORE_DATA))
		BOOST_THROW_EXCEPTION(std::runtime_error(
		boost::str(boost::format("NetServerEnum failed with error code: %d") % status)));

	std::cout << "::NetServerEnum found " << totalEntries << " network nodes" << std::endl;

	sFirst = sInfo;

	for (DWORD index = 0; index < entriesRead; index++)
	{
		try
		{
			if (sInfo == nullptr)
				BOOST_THROW_EXCEPTION(std::runtime_error("NetServerEnum returned unexpected null server info."));

			auto ip_address = Resolve(CW2CU(sInfo->sv101_name));
			auto mac_address = Arp(ip_address);

			//boost::shared_ptr<NetworkProtocol::HostInfo> hostinfo(new NetworkProtocol::HostInfo);
			//hostinfo->set_hostname(CW2CU(sInfo->sv101_name));
			std::cout << "\t" << CW2CU(sInfo->sv101_name) << std::endl;

			//hostinfo->set_description(CW2CU(sInfo->sv101_comment));
			std::cout << CW2CU(sInfo->sv101_comment) << std::endl;

			//hostinfo->set_ip_address(ip_address);
			std::cout << ip_address << std::endl;

			//hostinfo->set_mac_address(mac_address);
			std::cout << mac_address << std::endl;

			//hostinfo->set_type(((sInfo->sv101_type & SV_TYPE_WORKSTATION) || (sInfo->sv101_type & SV_TYPE_SERVER)) ? "pc" : "unknown");
			std::cout << (((sInfo->sv101_type & SV_TYPE_WORKSTATION) || (sInfo->sv101_type & SV_TYPE_SERVER)) ? "pc" : "unknown") << std::endl;

			//onHostFound_(hostinfo, scanId, false);
			sInfo++;
		}
		catch (const std::exception& e)
		{
			std::cerr << "NetBIOS scanner error: " << e.what() << std::endl;
		}
	}

	if (sFirst != nullptr)
		::NetApiBufferFree(sFirst);
}

std::string NetBIOSScanner::Resolve(const std::string& hostname)
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);

	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::tcp::v4(), hostname, "0");
	auto addresses = resolver.resolve(query);

	return addresses->endpoint().address().to_string();
}

std::string NetBIOSScanner::Arp(const std::string& ipAddress)
{
	ULONG mac_buffer[2] = { 0 };
	ULONG mac_buffer_size = 6;
	auto dest = ::inet_addr(ipAddress.c_str());
	CHECK_OS_ERROR_CODE(::SendARP(dest, 0, &mac_buffer, &mac_buffer_size));

	mac_buffer[0] = ::ntohl(mac_buffer[0]);
	mac_buffer[1] = ::ntohl(mac_buffer[1]) >> 16;

	std::string s = boost::str(boost::format("%|08X|%|04X|") % mac_buffer[0] % mac_buffer[1]);
	boost::algorithm::to_lower(s);
	return s;
}
