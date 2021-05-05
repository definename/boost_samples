#include "pch.h"

int _tmain(int argc, TCHAR* argv[])
{
	try
	{
		boost::filesystem::path p(argv[0]);
		p.remove_filename();
		p /= "version.cfg";

		std::cout << p.string() << std::endl;

		std::ifstream file(p.string().c_str());
		if (!file)
			throw std::runtime_error("Unable to open file");

		boost::property_tree::ptree pt;
		//boost::property_tree::ini_parser::read_ini(p.string().c_str(), pt);
		boost::property_tree::ini_parser::read_ini(file, pt);
		std::cout << pt.get<std::string>("version") << std::endl;
		std::cout << pt.get<std::string>("agent_installer_link") << std::endl;
		std::cout << pt.get<std::string>("md5") << std::endl;
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