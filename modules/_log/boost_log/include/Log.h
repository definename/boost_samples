#pragma once

namespace myLog
{
	// Here we define our application severity levels.
	enum severity_level
	{
		debug,
		info,
		warning,
		error,
		fatal
	};

	// The formatting logic for the severity level
	template< typename CharT, typename TraitsT >
	inline std::basic_ostream< CharT, TraitsT >& operator<< (
		std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
	{
		static const char* const str[] =
		{
			" [DEBUG] ",
			" [INFO] ",
			" [WARNING] ",
			" [ERROR] ",
			" [FATAL] "
		};
		if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
			strm << str[lvl];
		else
			strm << static_cast< int >(lvl);
		return strm;
	}

	typedef boost::log::sources::severity_logger_mt<severity_level> logger_t;
	BOOST_LOG_GLOBAL_LOGGER(my_logger, logger_t)
}

// Define macros through which we'll log.
#define LDBG_ BOOST_LOG_SEV(myLog::my_logger::get(), myLog::debug)
#define LAPP_ BOOST_LOG_SEV(myLog::my_logger::get(), myLog::info)
#define LWRN_ BOOST_LOG_SEV(myLog::my_logger::get(), myLog::warning)
#define LERR_ BOOST_LOG_SEV(myLog::my_logger::get(), myLog::error)
#define LFTL_ BOOST_LOG_SEV(myLog::my_logger::get(), myLog::fatal)