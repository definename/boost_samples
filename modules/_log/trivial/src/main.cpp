#include "pch.h"

#define LTRC_ BOOST_LOG_TRIVIAL(trace)
#define LDBG_ BOOST_LOG_TRIVIAL(debug)
#define LAPP_ BOOST_LOG_TRIVIAL(info)
#define LWRN_ BOOST_LOG_TRIVIAL(warning)
#define LERR_ BOOST_LOG_TRIVIAL(error)
#define LFTL_ BOOST_LOG_TRIVIAL(fatal)

void Init();

int main(int argc, char* argv[])
{
	try
	{
		Init();

		boost::thread_group group;
		group.create_thread([]() -> void { LTRC_ << "A trace severity message"; });
		group.create_thread([]() -> void { LDBG_ << "A debug severity message"; });
		group.create_thread([]() -> void { LAPP_ << "An informational severity message"; });
		group.create_thread([]() -> void { LWRN_ << "A warning severity message"; });
		group.create_thread([]() -> void { LERR_ << "An error severity message"; });
		group.create_thread([]() -> void { LFTL_ << "A fatal severity message"; });

		group.join_all();
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

void Init()
{
	boost::log::add_file_log(
		boost::log::keywords::filter = boost::log::trivial::severity >= boost::log::trivial::trace,
		boost::log::keywords::file_name = "logging.log",
		boost::log::keywords::open_mode = std::ios::out,
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format = boost::log::expressions::stream
		<< boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S:%f ")
		<< boost::log::expressions::message
		);

	boost::log::formatter formatter = boost::log::expressions::stream
		<< boost::log::expressions::format_date_time<boost::posix_time::ptime>(
		"TimeStamp", "[%Y-%m-%d %H:%M:%S:%f] ")
		<< boost::log::expressions::attr<boost::log::trivial::severity_level>("Severity")
		<< boost::log::expressions::message;

	boost::log::filter filter =
		boost::log::trivial::severity >= boost::log::trivial::trace;

	decltype(boost::log::add_console_log()) sink = boost::log::add_console_log();
	sink->set_filter(filter);
	sink->set_formatter(formatter);
	sink->locked_backend()->auto_flush(true);

	boost::log::add_common_attributes();
}