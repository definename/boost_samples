#pragma once

namespace opennat
{

class Errors
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Errors()
		: unreachable_("Destination host unreachable")
	{ }

	//
	// Public data members.
	//
public:
	//! Host unreachable.
	const std::string unreachable_;
};

}