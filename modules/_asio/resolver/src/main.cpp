#include "pch.h"
#include "Resolver.h"

int _tmain(int argc, TCHAR* argv[])
{
	try
	{
		setlocale(LC_ALL, "");
		//! Represents functionality to lookup local hostname.
		_tstring host = boost::asio::ip::host_name();
		_tcout << _T("Local host: ") << host << std::endl;

		//! Represents functionality to lookup hostname/address by given hostname/address.
		Resolver r;
		r.Resolve(host);

		MSG msg;
		while (!GetMessage(&msg, NULL, NULL, NULL))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	catch (std::exception& e)
	{
		_tcerr << "Error occurred: " << e.what() << std::endl;

		_twostringstream error;
		error << _T("Error: ") << e.what() << _T("\nUsage: resolver.exe oleg-pc");
		::MessageBox(NULL, error.str().c_str(), _T("Error occurred"), MB_ICONERROR);
	}

	return 0;
}