#include "pch.h"
#include "nat/Client.h"
#include "nat/Kernel.h"
#include "nat/Log.h"
#include "nat/Server.h"
#include "nat/SessionMgr.h"
#include "nat/ProxyMgr.h"
#include "nat/Errors.h"
#include "nat/SubscribeMgr.h"

namespace opennat
{

Kernel::Kernel()
	: errors_(new Errors)
{ }

Kernel::~Kernel()
{ }

types::ClientPtr Kernel::GetClient(const boost::uuids::uuid& id) const
{
	return boost::make_shared<Client>(id, boost::make_shared<SessionMgr>());
}

const types::ErrorsPtr Kernel::GetError() const
{
	return errors_;
}

types::ServerPtr Kernel::GetServer() const
{
	return boost::make_shared<Server>(
		boost::make_shared<SessionMgr>(),
		boost::make_shared<ProxyMgr>(),
		boost::make_shared<SubscribeMgr>());
}

void Kernel::InitLog(
	const boost::filesystem::path& historyPath,
	const boost::filesystem::path& logPath) const
{
	boost::log::add_file_log(
		boost::log::keywords::filter = log::severity >= log::SeverityLevel::Trace,
		boost::log::keywords::file_name = logPath.string(),
		boost::log::keywords::open_mode = std::ios::app,
		boost::log::keywords::target = historyPath.string(),
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,
		boost::log::keywords::max_size = 50 * 1024 * 1024,
		boost::log::keywords::min_free_space = 100 * 1024 * 1024,
		boost::log::keywords::scan_method = boost::log::sinks::file::scan_matching,
		boost::log::keywords::auto_flush = true,
		boost::log::keywords::format =
		boost::log::expressions::format("%1% [%2%] %3%")
		% boost::log::expressions::format_date_time(log::timestamp, "%Y-%m-%d %H:%M:%S:%f")
		% log::severity
		% boost::log::expressions::message
		);
}

}