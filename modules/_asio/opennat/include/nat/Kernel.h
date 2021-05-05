#pragma once

#include "nat/IKernel.h"

namespace opennat
{

class Kernel
	: public IKernel
	, public boost::serialization::singleton<Kernel>
{
	//
	// Construction and destruction.
	//
private:
	//! Constructor.
	Kernel();
	//! Destructor.
	~Kernel();

	//
	// Private interface.
	//
private:
	//! Allows to access the member function.
	friend class boost::serialization::detail::singleton_wrapper<Kernel>;

	//
	// Public interface
	//
public:
	//! Returns NAT client pointer.
	types::ClientPtr GetClient(const boost::uuids::uuid& id) const;
	//! Returns error pointer.
	const types::ErrorsPtr GetError() const;
	//! Returns server pointer.
	types::ServerPtr GetServer() const;
	//! Initializes logging.
	void InitLog(
		const boost::filesystem::path& historyPath,
		const boost::filesystem::path& logPath) const;

	//
	// Private data member.
	//
private:
	//! Error pointer.
	const types::ErrorsPtr errors_;
};

}