#include "pch.h"
#include "log.h"

void init()
{
	
	//const std::string filename = "boost_%Nlog.log";
	const std::string filename = "C:\\ProgramData\\boost_%Nlog.log";
	//const std::string filename = "my_log_%N.log";
	//const std::string filename = "file_%Y%m%d.log";
	//const std::string filename = "file_%Y-%m-%d_%H-%M-%S.%N.log";
	boost::log::add_file_log(
		boost::log::keywords::filter = boost::log::expressions::attr<myLog::severity_level>("Severity") >= myLog::debug && boost::log::expressions::attr<myLog::severity_level>("Severity") <= myLog::fatal,
		boost::log::keywords::file_name = filename,
		boost::log::keywords::open_mode = std::ios::app,
		//boost::log::keywords::rotation_size = 1024,
		//boost::log::keywords::target = "log_history",
		//boost::log::keywords::max_size = 2 * 1024,
		//boost::log::keywords::min_free_space = 50 * 1024 * 1024,
		boost::log::keywords::scan_method = boost::log::sinks::file::scan_all,
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format = (
		boost::log::expressions::stream << boost::log::expressions::format_date_time<boost::posix_time::ptime >("TimeStamp", "%d-%m-%Y %H:%M:%S:%f")
		<< boost::log::expressions::attr<myLog::severity_level>("Severity")
		<< boost::log::expressions::smessage));

	boost::log::add_console_log
		(
		std::cout,
		boost::log::keywords::filter = boost::log::expressions::attr<myLog::severity_level>("Severity") >= myLog::debug && boost::log::expressions::attr<myLog::severity_level>("Severity") <= myLog::fatal,
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format = (
		boost::log::expressions::stream << boost::log::expressions::format_date_time<boost::posix_time::ptime >("TimeStamp", "%d-%m-%Y %H:%M:%S:%f")
		<< boost::log::expressions::attr<myLog::severity_level>("Severity")
		<< boost::log::expressions::smessage)
		);
}

int main()
{
	try
	{
		init();

		while (true)
		{
			LDBG_ << "Yep! debug debug debug debug debug debug debug debug debug debug";
			LAPP_ << "Yep! info info info info info info info info info info info info info";
			LWRN_ << "Yep! warning warning warning warning warning warning warning warning";
			LERR_ << "Yep! error error error error error error error error error error error";
			LFTL_ << "Yep! fatal fatal fatal fatal fatal fatal fatal fatal fatal fatal";
		}
	}
	catch (const std::exception& e)
	{
		_tcerr << "Error occurred: " << e.what() << std::endl;

		_twostringstream error;
		error << _T("Error: ") << e.what();
		::MessageBox(NULL, error.str().c_str(), _T("Error occurred"), MB_ICONERROR);
	}

	return 0;
}