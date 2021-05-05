#include "pch.h"
#include "log.h"

namespace myLog
{
	BOOST_LOG_GLOBAL_LOGGER_INIT(my_logger, logger_t)
	{
		logger_t lg;
		lg.add_attribute("TimeStamp", boost::log::attributes::local_clock());

		return lg;
	}
}