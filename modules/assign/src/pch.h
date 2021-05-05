#pragma once

//////////////////////////////////////////////////////////////////////////
// STL includes and definitions.
//

#include <iostream>
#include <tchar.h>
#include <sstream>
#include <map>

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
// Boost includes and definitions.
//

#include "boost/assign.hpp"
#include "boost/assign/list_of.hpp"
#include <boost/assert.hpp>

//////////////////////////////////////////////////////////////////////////
// Platform specific includes and definitions.
//

#include <Windows.h>