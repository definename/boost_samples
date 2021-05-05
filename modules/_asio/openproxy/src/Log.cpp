#include "pch.h"
#include "proxy/Log.h"

namespace proxy
{
namespace log
{

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, logger_mt)
{
	boost::log::add_file_log(
		boost::log::keywords::filter = severity >= SeverityLevel::Trace,
		boost::log::keywords::file_name = "logging_%N.log",
		boost::log::keywords::open_mode = std::ios::app,
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::target = "history",
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
		boost::log::keywords::max_size = 50 * 1024 * 1024,
		boost::log::keywords::min_free_space = 100 * 1024 * 1024,
		boost::log::keywords::scan_method = boost::log::sinks::file::scan_matching,
		boost::log::keywords::format =
		boost::log::expressions::format("%1% [%2%] %3%")
		% boost::log::expressions::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S:%f")
		% severity
		% boost::log::expressions::message
		);

	boost::log::add_console_log(
		std::cout,
		boost::log::keywords::filter = severity >= SeverityLevel::Trace,
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format =
		boost::log::expressions::format("%1% [%2%] %3%")
		% boost::log::expressions::format_date_time(timestamp, "%Y-%m-%d %H:%M:%S:%f")
		% severity
		% boost::log::expressions::message
		);

	logger_mt lg;

	lg.add_attribute(timestamp.get_name(), boost::log::attributes::local_clock());

	return lg;
}

}
}