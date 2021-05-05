#include "pch.h"
#include "proxy/Login.h"
#include "proxy/Greeting.h"
#include "proxy/Event.h"

namespace proxy
{
namespace protocol
{

Event::Event()
	: has_greeting_(false)
	, has_login_(false)
{ }

Event::Event(const Greeting& greeting, const Login& login)
	: greeting_(greeting)
	, login_(login)
	, has_greeting_(false)
	, has_login_(false)
{ }

Event::~Event()
{ }

const Greeting& Event::greeting()
{
	if (!has_greeting())
		BOOST_THROW_EXCEPTION(std::runtime_error("There is no greeting"));
	return greeting_;
}

const Login& Event::login()
{
	if (!has_login())
		BOOST_THROW_EXCEPTION(std::runtime_error("There is no login"));
	return login_;
}

void Event::set_login(const Login& login)
{
	set_has_login();
	login_ = login;
}

void Event::set_greeting(const Greeting& greeting)
{
	set_has_greeting();
	greeting_ = greeting;
}

bool Event::has_login() const
{
	return has_login_;
}

bool Event::has_greeting() const
{
	return has_greeting_;
}

void Event::set_has_login()
{
	has_login_ = true;
}

void Event::set_has_greeting()
{
	has_greeting_ = true;
}

}

}