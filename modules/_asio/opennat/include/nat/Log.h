#pragma once

namespace opennat
{
namespace log
{

// Severity level.
enum SeverityLevel
{
	Trace,
	Debug,
	Info,
	Warning,
	Error,
	Fatal
};

// The formatting logic for the severity level
template <typename TChar, typename TTraits>
inline std::basic_ostream<TChar, TTraits>& operator<< (
	std::basic_ostream<TChar, TTraits>& strm, SeverityLevel lvl)
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

// Defines attribute placeholders.
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", SeverityLevel)

// Logger type.
using logger_mt = boost::log::sources::severity_logger_mt<SeverityLevel>;
//  Logger declaration.
BOOST_LOG_GLOBAL_LOGGER(opennat_logger, logger_mt)

}

// Define macros through which we'll log.
#define LTRC_ BOOST_LOG_SEV(opennat::log::opennat_logger::get(), opennat::log::Trace)
#define LDBG_ BOOST_LOG_SEV(opennat::log::opennat_logger::get(), opennat::log::Debug)
#define LAPP_ BOOST_LOG_SEV(opennat::log::opennat_logger::get(), opennat::log::Info)
#define LWRN_ BOOST_LOG_SEV(opennat::log::opennat_logger::get(), opennat::log::Warning)
#define LERR_ BOOST_LOG_SEV(opennat::log::opennat_logger::get(), opennat::log::Error)
#define LFTL_ BOOST_LOG_SEV(opennat::log::opennat_logger::get(), opennat::log::Fatal)

}