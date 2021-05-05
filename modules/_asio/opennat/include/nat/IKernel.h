#pragma once

#include "nat/Types.h"

namespace opennat
{

class IKernel
{
	//
	// Construction and destruction.
	//
protected:
	//! Destructor.
	virtual ~IKernel() { }

	//
	// Public interface
	//
public:
	//! Returns client pointer.
	virtual types::ClientPtr GetClient(const boost::uuids::uuid& id) const = 0;
	//! Returns error pointer.
	virtual const types::ErrorsPtr GetError() const = 0;
	//! Returns server pointer.
	virtual types::ServerPtr GetServer() const = 0;
	//! Initializes logging.
	virtual void InitLog(
		const boost::filesystem::path& historyPath,
		const boost::filesystem::path& logPath) const = 0;
};

}