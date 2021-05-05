#include "pch.h"
#include "proxy/Greeting.h"

namespace proxy
{
namespace protocol
{

Greeting::Greeting()
	: ready_(false)
	, has_ready_(false)
{ }

Greeting::~Greeting()
{ }

bool Greeting::ready()
{
	if (!has_ready())
		BOOST_THROW_EXCEPTION(std::runtime_error("There is no ready"));
	return ready_;
}

void Greeting::set_ready()
{
	set_has_ready();
	ready_ = true;
}

bool Greeting::has_ready() const
{
	return has_ready_;
}

void Greeting::set_has_ready()
{
	has_ready_ = true;
}

}
}