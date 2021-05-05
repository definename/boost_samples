#include "pch.h"
#include "ICMPScanner.h"

int _tmain(int argc, TCHAR* argv[])
{
	try
	{
		ICMPScanner scanner;
		scanner.Scan(
			asio::ip::address::from_string("192.168.25.0"),
			asio::ip::address::from_string("255.255.255.0"));

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
		error << _T("Error: ") << e.what();
		::MessageBox(NULL, error.str().c_str(), _T("Error occurred"), MB_ICONERROR);
	}

	return 0;
}