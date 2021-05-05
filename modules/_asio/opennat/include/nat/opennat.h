#pragma once

#include "boost/log/common.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/utility/setup.hpp"
#include "boost/log/support/date_time.hpp"


#include "nat/Errors.h"
#include "nat/IClient.h"
#include "nat/IKernel.h"
#include "nat/Log.h"
#include "nat/IServer.h"
#include "nat/ISessionMgr.h"
#include "nat/IProxyMgr.h"
#include "nat/Message.h"
#include "nat/ISubscribeMgr.h"


namespace opennat
{

//! Returns kernel reference.
const IKernel& GetKernel();

}