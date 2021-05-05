#pragma once

//////////////////////////////////////////////////////////////////////////
// STL includes and definitions.
//

#include <iostream>
#include <queue>

//////////////////////////////////////////////////////////////////////////
// Boost includes and definitions.
//

#include "boost/asio.hpp"
#include "boost/algorithm/string/predicate.hpp"
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/program_options.hpp"
#include "boost/signals2.hpp"
#include "boost/serialization/singleton.hpp"
#include "boost/scope_exit.hpp"
#include "boost/thread.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_serialize.hpp"
#include "boost/unordered_map.hpp"
#include "boost/unordered_set.hpp"

//////////////////////////////////////////////////////////////////////////
// Open nat includes and definitions.
//

#include "nat/opennat.h"

//////////////////////////////////////////////////////////////////////////
// Miniupnp includes and definitions.
//

#include "miniupnpc/miniupnpc.h"
#include "miniupnpc/upnpcommands.h"
#include "miniupnpc/upnperrors.h"

//////////////////////////////////////////////////////////////////////////
// Platform specific includes and definitions.
//

#include <windows.h>
