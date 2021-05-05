#include "pch.h"
#include "nat/UPNPCore.h"

namespace opennat
{

UPNPCore::UPNPCore()
{ }

UPNPCore::~UPNPCore()
{
	Disconnect();
}

void UPNPCore::Connect()
{
	int error;
	struct UPNPDev* devList = upnpDiscover(2000, nullptr, nullptr, UPNP_LOCAL_PORT_ANY, 0, 2, &error);
	if (devList)
	{
		struct UPNPUrls urls_;
		struct IGDdatas data_;
		if (UPNP_GetValidIGD(devList, &urls_, &data_, localAddr_.data(), localAddr_.size()) == 1)
		{
			controlURL_.assign(urls_.controlURL);
			serviceType_.assign(data_.first.servicetype);

			isConnect_ = true;

			FreeUPNPUrls(&urls_);
		}
		else
		{
			LERR_ << "UPnP Internet Gateway Device hasn't been discovered";
		}
		freeUPNPDevlist(devList);
		devList = nullptr;
	}
	else
	{
		LERR_ << "UPnP devices haven't been discovered: " << strupnperror(error);
	}
}

bool UPNPCore::IsConnected() const
{
	return isConnect_;
}

void UPNPCore::Disconnect()
{
	isConnect_ = false;
	std::array<char, 64>().swap(localAddr_);
	controlURL_.clear();
	serviceType_.clear();
}

}