#include "pch.h"
#include "nat/Log.h"

namespace opennat
{
namespace log
{

BOOST_LOG_GLOBAL_LOGGER_INIT(opennat_logger, logger_mt)
{
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