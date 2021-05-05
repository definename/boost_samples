#include "pch.h"

//! Generates uuid from given bytes string.
std::string Generate(std::string const& bytes);
//! Returns valid drives in the system.
std::vector<std::wstring> GetDrives(DWORD type);
//! Returns the Ethernet address of the first Ethernet adapter found on the system.
std::string GetMac();
//! Returns machine guid.
std::wstring GetMachineGuidFromRegistry(REGSAM mask);
//! Converts between UTF-8 and UTF-16
std::string WideToNarrow(std::wstring const& in);

int main()
{
	auto drives(GetDrives(DRIVE_FIXED));
	if (drives.empty())
		drives.push_back(L"C:\\");

	DWORD serial = 0;
	::GetVolumeInformation(
		drives[0].c_str(), nullptr, MAX_PATH, &serial, nullptr, nullptr, nullptr, MAX_PATH);

	auto guid = GetMachineGuidFromRegistry(0);
	if (guid.size() == 0)
		guid = GetMachineGuidFromRegistry(KEY_WOW64_64KEY);

	std::string mac(GetMac());
	boost::algorithm::replace_all(mac, ":", "");

	std::cout << boost::uuids::to_string(boost::uuids::string_generator()(
		Generate(std::to_string(serial) + WideToNarrow(guid) + mac))) << std::endl;

	return 0;
}

std::string GetMac()
{
	unsigned char id[6];
	std::memset(&id, 0, sizeof(id));

	PIP_ADAPTER_INFO pAdapterInfo;
	ULONG len = sizeof(IP_ADAPTER_INFO);
	pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[len]);

	DWORD rc = ::GetAdaptersInfo(pAdapterInfo, &len);
	if (rc == ERROR_BUFFER_OVERFLOW)
	{
		delete[] reinterpret_cast<char*>(pAdapterInfo);
		pAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new char[len]);
	}

	PIP_ADAPTER_INFO pAdapter = 0;
	if (::GetAdaptersInfo(pAdapterInfo, &len) == NO_ERROR)
	{
		pAdapter = pAdapterInfo;
		bool found = false;
		while (pAdapter && !found)
		{
			if (pAdapter->Type == MIB_IF_TYPE_ETHERNET && pAdapter->AddressLength == sizeof(id))
			{
				found = true;
				std::memcpy(&id, pAdapter->Address, pAdapter->AddressLength);
			}
			pAdapter = pAdapter->Next;
		}
	}
	delete[] reinterpret_cast<char*>(pAdapterInfo);

	std::string result(18, 0);
	std::sprintf(&result[0], "%02x:%02x:%02x:%02x:%02x:%02x", id[0], id[1], id[2], id[3], id[4], id[5]);

	return result;
}

std::vector<std::wstring> GetDrives(DWORD type)
{
	WCHAR buf[MAX_PATH] = {};
	std::vector<std::wstring> out;
	DWORD len = ::GetLogicalDriveStrings(MAX_PATH, buf);
	if (len == 0)
		return out;

	size_t counter = 0;
	do
	{
		std::wstring drive = &buf[counter];
		counter += drive.size() + 1;

		if (drive.empty())
			break;

		if (type == ::GetDriveType(drive.c_str()))
			out.push_back(drive);

	} while (counter < len);

	return out;
}

std::wstring GetMachineGuidFromRegistry(REGSAM mask)
{
	CRegKey key;
	if (ERROR_SUCCESS == key.Open(
		HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", KEY_READ | mask))
	{
		ULONG sizeBuf;
		if (ERROR_SUCCESS == key.QueryStringValue(L"MachineGuid", NULL, &sizeBuf))
		{
			std::wstring guidBuffer;
			guidBuffer.resize(sizeBuf);
			if (ERROR_SUCCESS == key.QueryStringValue(L"MachineGuid", &guidBuffer[0], &sizeBuf))
				return guidBuffer;
		}
	}
	return L"";
}

std::string Generate(std::string const& bytes)
{
	boost::uuids::detail::sha1 sha;
	sha.process_bytes(bytes.data(), bytes.size());
	unsigned int dig[5] = {};
	sha.get_digest(dig);

	return boost::str(boost::format(
		"%08x%08x%08x%08x%08x") % dig[0] % dig[1] % dig[2] % dig[3] % dig[4]);
}

std::string WideToNarrow(std::wstring const& in)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(in);
}