#include "pch.h"

void TestAssignMap();

void _tmain(int argc, TCHAR* argv[])
{
	try
	{
		TestAssignMap();
	}
	catch (const std::exception& e)
	{
		_tcerr << "Error occurred: " << e.what() << std::endl;

		_twostringstream error;
		error << _T("Error: ") << e.what();
		::MessageBox(NULL, error.str().c_str(), _T("Error occurred"), MB_ICONERROR);
	}
}

void TestAssignMap()
{
	//! Map container type.
	typedef std::map<_tstring, int> Container;
	//! Map container.
	Container container1 = boost::assign::map_list_of
		(_T("one"), 1)
		(_T("two"), 2)
		(_T("three"), 3);
	for (const Container::value_type& val: container1)
	{
		_tcout << val.second << _T(" ");
	}
	_tcout << std::endl;

	Container container2 = boost::assign::list_of< std::pair<_tstring, int> >
		(_T("one"), 1)
		(_T("two"), 2)
		(_T("three"), 3);

	for (const Container::value_type& val : container2)
	{
		_tcout << val.second << _T(" ");
	}
	_tcout << std::endl;
}