#pragma once

#include "targetver.h"

//////////////////////////////////////////////////////////////////////////
// STL includes and definitions.
// 

#include <iostream>
#include <tchar.h>

#ifdef _UNICODE
#define _tcout std::wcout
#define _tcin std::wcin
#define _tcerr std::wcerr
#define _tstring std::wstring
#define _tostream std::wostream
#define _tistream std::wistream
#define _twstringstream std::wstringstream
#define _twostringstream std::wostringstream
#define _twistringstream std::wistringstream
#else
#define _tcin std::cin
#define _tcout std::cout
#define _tcerr std::cerr
#define _tstring std::string
#define _tostream std::ostream
#define _tistream std::istream
#define _twstringstream std::stringstream
#define _twostringstream std::ostringstream
#define _twistringstream std::istringstream
#endif

//////////////////////////////////////////////////////////////////////////
// Boost include and definitions.
//

//! Selects the target Windows version in order to prevent mismatch errors.
//#define BOOST_USE_WINAPI_VERSION BOOST_WINAPI_VERSION_WIN6

#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/shared_ptr.hpp>

//////////////////////////////////////////////////////////////////////////
// Platform specific includes and definitions.
//

#include <windows.h>