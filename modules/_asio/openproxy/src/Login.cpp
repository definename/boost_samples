#include "pch.h"
#include "proxy/Login.h"

namespace proxy
{
namespace protocol
{

Login::Login()
	: client_id_(boost::uuids::nil_uuid())
	, session_id_(boost::uuids::nil_uuid())
	, type_(protocol::UNKNOWN)
	, has_client_id_(false)
	, has_session_id_(false)
	, has_type_(false)
{ }

Login::~Login()
{ }

boost::uuids::uuid Login::client_id()
{
	if (!has_client_id())
		BOOST_THROW_EXCEPTION(std::runtime_error("There is no client id"));
	return client_id_;
}

boost::uuids::uuid Login::session_id()
{
	if (!has_session_id())
		BOOST_THROW_EXCEPTION(std::runtime_error("There is no session id"));
	return session_id_;
}

protocol::StreamType Login::type()
{
	if (!has_type())
		BOOST_THROW_EXCEPTION(std::runtime_error("There is no type"));
	return type_;
}

void Login::set_client_id(const boost::uuids::uuid& id)
{
	set_has_client_id();
	client_id_ = id;
}

void Login::set_session_id(const boost::uuids::uuid& id)
{
	set_has_session_id();
	session_id_ = id;
}

void Login::set_type(const protocol::StreamType type)
{
	set_has_type();
	type_ = type;
}

bool Login::has_client_id() const
{
	return has_client_id_;
}

bool Login::has_session_id() const
{
	return has_session_id_;
}

bool Login::has_type() const
{
	return has_type_;
}

void Login::set_has_client_id()
{
	has_client_id_ = true;
}

void Login::set_has_session_id()
{
	has_session_id_ = true;
}

void Login::set_has_type()
{
	has_type_ = true;
}

}
}