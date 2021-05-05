#include "pch.h"
#include "nat/UPNPClient.h"

namespace opennat
{

UPNPClient::UPNPClient()
	: isStarted_(false)
{
	Run();
}

UPNPClient::~UPNPClient()
{
	try
	{
		Stop();
	}
	catch (const std::exception& e)
	{
		LERR_ << "UPNP client destruction error: " << e.what();
	}
}

void UPNPClient::Start()
{
	if (!IsRunning())
	{
		LERR_ << "UPNP core service is not running...";
		return;
	}

	ioService_->post([this]() -> void
	{
		UPNPCore::Connect();
		isStarted_ = true;
		onStarted_();
	});
}

bool UPNPClient::IsStarted() const
{
	return isStarted_;
}

void UPNPClient::Stop()
{
	if (!IsStarted())
	{
		LERR_ << "UPNP client is not started";
		return;
	}

	LDBG_ << "UPNP client is stopping...";
	{
		isStarted_ = false;
		boost::mutex::scoped_lock l(mutex_);
		for (const auto& val : upnpHistory_)
		{
			try
			{
				LDBG_ << "Remove mapped port: " << val;
				DeletePortMapping(boost::lexical_cast<std::string>(val));
			}
			catch (const boost::bad_lexical_cast& e)
			{
				LERR_ << "UPNP cleanup error: " << e.what();
			}
		}
		upnpHistory_.clear();
		UPNPCore::Disconnect();
	}
	Core::Stop();
}

bool UPNPClient::DoAddPortMapping(
	const char* url,
	const char* service,
	const std::string& extPort,
	const std::string& inPort,
	const std::string& inAddr,
	const std::string& desc)
{
	auto ret = UPNP_AddPortMapping(
		url,
		service,
		extPort.data(),
		inPort.data(),
		inAddr.data(),
		desc.data(),
		"tcp",
		nullptr,
		nullptr);
	if (ret == UPNPCOMMAND_SUCCESS)
	{
		if (IsMapped(url, service, extPort, inPort, inAddr))
			return true;
	}

	LERR_ << "Port mapping failed: " << strupnperror(ret);
	return false;
}

boost::signals2::connection UPNPClient::DoOnStarted(const OnStarted::slot_type& signal)
{
	return onStarted_.connect(signal);
}

unsigned short UPNPClient::AddPortMapping(const unsigned short inPort)
{
	boost::mutex::scoped_lock l(mutex_);
	if (!IsConnected())
	{
		LERR_ << "UPNP client is not running...";
		return UPNPCOMMAND_PORTMAPPING_ERROR;
	}

	Redirections redirects;
	GetPortMappingEntries(controlURL_.data(), serviceType_.data(), redirects);

	auto isLocalEp = std::find_if(
		redirects.begin(),
		redirects.end(),
		[this, &inPort](Entry entry) -> bool
	{
		return (entry.inPort_ == inPort &&
			boost::iequals(entry.inAddr_, localAddr_.data()) &&
			boost::iequals(entry.protocol_, "tcp") ? true : false);
	});

	if (isLocalEp != redirects.end())
	{
		return isLocalEp->extPort_;
	}
	else
	{
		for (int extPort = 49152; extPort != 65535; ++extPort)
		{
			auto isExtPort = std::find_if(
				redirects.begin(),
				redirects.end(),
				[&extPort](Entry entry) -> bool
			{
				return entry.extPort_ == extPort;
			});

			if (isExtPort == redirects.end())
			{
				if (DoAddPortMapping(
					controlURL_.data(),
					serviceType_.data(),
					std::to_string(extPort),
					std::to_string(inPort),
					localAddr_.data(),
					"opennat"
					))
				{
					LDBG_ << "Added port mapping ext: " << extPort << " -> in: " << inPort;
					upnpHistory_.insert(extPort);
					return extPort;
				}
				break;
			}
		}
	}
	return UPNPCOMMAND_PORTMAPPING_ERROR;
}

bool UPNPClient::DeletePortMapping(const std::string& extPort)
{
	if (!IsConnected())
	{
		LERR_ << "UPNP client is not running...";
		return false;
	}

	auto ret = UPNP_DeletePortMapping(
		controlURL_.data(), serviceType_.data(), extPort.data(), "tcp", nullptr);
	if (ret == UPNPCOMMAND_SUCCESS)
		return true;

	LERR_ << "Unable to delete mapped port: " << strupnperror(ret);
	return false;
}

void UPNPClient::GetPortMappingEntries(const char* url, const char* service, Redirections& redirects)
{
	int ret;
	int index = 0;
	do
	{
		std::array<char, 6> extPort;
		std::array<char, 64> inAddr;
		std::array<char, 6> inPort;
		std::array<char, 4> protocol;
		std::array<char, 80> desc;
		ret = UPNP_GetGenericPortMappingEntry(url, service,
			std::to_string(index).data(),
			extPort.data(),
			inAddr.data(),
			inPort.data(),
			protocol.data(),
			desc.data(),
			nullptr,
			nullptr,
			nullptr);
		if (ret == UPNPCOMMAND_SUCCESS)
		{
			try
			{
				Entry r;
				r.extPort_ = boost::lexical_cast<unsigned short>(extPort.data());
				r.inPort_ = boost::lexical_cast<unsigned short>(inPort.data());
				r.inAddr_.assign(inAddr.data());
				r.protocol_.assign(protocol.data());
				r.desc_.assign(desc.data());
				r.index_ = index;

				LDBG_ << boost::str(boost::format("%d %s %d -> %s:%d '%s'")
					% r.index_
					% r.protocol_
					% r.extPort_
					% r.inAddr_
					% r.inPort_
					% r.desc_);

				redirects.push_back(r);
			}
			catch (const boost::bad_lexical_cast & e)
			{
				LERR_ << "Redirections error" << e.what();
			}

			index++;
			continue;
		}
		else if (ret != 713)
		{
			LERR_ << "List redirections error: " << strupnperror(ret);
		}
	}
	while (ret == UPNPCOMMAND_SUCCESS);
}

std::string UPNPClient::GetExternalIP() const
{
	if (!IsConnected())
	{
		LERR_ << "UPNP client is not running...";
		return std::string();
	}

	std::array<char, 64> extAddr;
	auto ret = UPNP_GetExternalIPAddress(controlURL_.data(), serviceType_.data(), extAddr.data());
	if (ret == UPNPCOMMAND_SUCCESS)
		return std::string().assign(extAddr.data());

	LDBG_ << "Unable to obtain external ip address: " << strupnperror(ret);
	return std::string();
}

std::string UPNPClient::GetLocalIP() const
{
	if (!IsConnected())
	{
		LERR_ << "UPNP client is not running...";
		return std::string();
	}

	return std::string().assign(localAddr_.data());
}

bool UPNPClient::IsMapped(
	const char* url,
	const char* service,
	const std::string& extPort,
	const std::string& inPort,
	const std::string& inAddr)
{
	char mappedaddr[64];
	char mappedport[6];
	auto ret = UPNP_GetSpecificPortMappingEntry(url, service,
		extPort.data(),
		"tcp",
		nullptr,
		mappedaddr,
		mappedport,
		nullptr,
		nullptr,
		nullptr);

	if (ret == UPNPCOMMAND_SUCCESS)
	{
		if (boost::iequals(mappedaddr, inAddr.data()) &&
			boost::iequals(mappedport, inPort.data()))
			return true;
	}

	LDBG_ << "Port mapping failed: " << strupnperror(ret);
	return false;
}

}