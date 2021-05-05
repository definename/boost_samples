#include "pch.h"
#include "NetBIOSScanner.h"

int _tmain(int argc, TCHAR* argv[])
{
	try
	{
		NetBIOSScanner scanner;
		scanner.Scan();

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