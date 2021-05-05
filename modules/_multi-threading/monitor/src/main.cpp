#include "pch.h"
#include "Monitor.h"

int _tmain(int argc, TCHAR* argv[])
{
	try
	{
		Monitor m;
		boost::thread t;
		t.join();

		t = boost::thread(boost::bind(&Monitor::StartMonitor, &m));

		while (!m.IsRunning())
		{
			if (!t.joinable() || t.try_join_for(boost::chrono::seconds(1)))
				break;
		}
		m.Stop();
		t.join();
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