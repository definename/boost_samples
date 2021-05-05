#include "pch.h"

namespace emulator
{

enum severity_level
{
	trace,
	debug,
	info,
	warning,
	error,
	fatal
};

// The formatting logic for the severity level
template <typename CharT, typename TraitsT>
inline std::basic_ostream<CharT, TraitsT>& operator<< (
	std::basic_ostream<CharT, TraitsT>& strm, severity_level lvl)
{
	static const char* const str[] =
	{
		"TRACE",
		"DEBUG",
		"INFO",
		"WARNING",
		"ERROR",
		"FATAL"
	};

	if (static_cast<std::size_t>(lvl) < (sizeof(str) / sizeof(*str)))
		strm << str[lvl];
	else
		strm << static_cast<int>(lvl);
	return strm;
}

void Init()
{
	decltype(boost::log::add_console_log()) sink = boost::log::add_console_log();
	sink->set_formatter(
		boost::log::expressions::format("%1% [%2%] <%3%> %4%")
		% boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%Y-%m-%d %H:%M:%S:%f")
		% boost::log::expressions::attr<emulator::severity_level>("Severity")
		% boost::log::expressions::attr<std::string>("Channel")
		% boost::log::expressions::message);
	sink->locked_backend()->auto_flush(true);

	boost::log::add_common_attributes();
}

}

//! Logging function.
void LoggingFunction(
	boost::log::sources::severity_channel_logger_mt<emulator::severity_level, std::string>& lg,
	const std::string& ch);


int main(int argc, char* argv[])
{
	try
	{
		emulator::Init();

		boost::log::sources::severity_channel_logger_mt<emulator::severity_level, std::string> lg;
		LoggingFunction(lg, "General");
		LoggingFunction(lg, "GUI");
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return EXIT_SUCCESS;
}

void LoggingFunction(
	boost::log::sources::severity_channel_logger_mt<emulator::severity_level, std::string>& lg,
	const std::string& ch)
{
	BOOST_LOG_CHANNEL_SEV(lg, ch, emulator::trace) << "A trace message";
	BOOST_LOG_CHANNEL_SEV(lg, ch, emulator::debug) << "A debug message";
	BOOST_LOG_CHANNEL_SEV(lg, ch, emulator::info) << "A info message";
	BOOST_LOG_CHANNEL_SEV(lg, ch, emulator::warning) << "An warning message";
	BOOST_LOG_CHANNEL_SEV(lg, ch, emulator::error) << "A error message";
	BOOST_LOG_CHANNEL_SEV(lg, ch, emulator::fatal) << "A fatal message";
}