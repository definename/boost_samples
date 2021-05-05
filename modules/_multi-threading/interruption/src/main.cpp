#include "pch.h"

class Loop
{
	//! Construction and destruction.
public:
	//! Constructor.
	Loop() { }
	//! Destructor.
	~Loop() { }

public:
	//! Loop function.
	void LoopFunc()
	{
		try
		{
			while (true)
			{
				boost::this_thread::interruption_point();
				std::cout << "Loop..." << std::endl;
			}
		}
		catch (const boost::thread_interrupted&)
		{
			std::cerr << "Loop function interrupted" << std::endl;
		}
		catch (const std::exception& e)
		{
			std::cerr << "Never will reach here: " << e.what() << std::endl;
		}
	}
};

int _tmain(int argc, TCHAR* argv[])
{
	try
	{
		Loop l;
		boost::thread t;
		t = boost::thread(boost::bind(&Loop::LoopFunc, &l));
		t.interrupt();
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