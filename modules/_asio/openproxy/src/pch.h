#pragma once

//////////////////////////////////////////////////////////////////////////
// STL includes and definitions.
//

#include <iostream>
#include <sstream>
#include <queue>

//////////////////////////////////////////////////////////////////////////
// Boost includes and definitions.
//

#include "boost/program_options.hpp"
#include "boost/serialization/singleton.hpp"
#include "boost/asio.hpp"
#include "boost/algorithm/string.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/make_shared.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/signals2.hpp"
#include "boost/thread.hpp"
#include "boost/unordered_map.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_serialize.hpp"

//////////////////////////////////////////////////////////////////////////
// Open proxy include and definitions.
//

#include "proxy/openproxy.h"

//////////////////////////////////////////////////////////////////////////
// Platform specific includes and definitions.
//

#include <windows.h>