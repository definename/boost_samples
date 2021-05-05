#pragma once

#include "proxy/Types.h"

namespace proxy
{
namespace protocol
{

class Login
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Login();
	//! Destructor.
	~Login();

	//
	// Public interface.
	//
public:
	//! Returns client id.
	boost::uuids::uuid client_id();
	//! Returns session id.
	boost::uuids::uuid session_id();
	//! Returns type.
	protocol::StreamType type();
	//! Sets client id.
	void set_client_id(const boost::uuids::uuid& id);
	//! Sets session id.
	void set_session_id(const boost::uuids::uuid& id);
	//! Sets type.
	void set_type(const protocol::StreamType type);
	//! Has client id.
	bool has_client_id() const;
	//! Has session id.
	bool has_session_id() const;
	//! Has type.
	bool has_type() const;


	//
	// Private interface.
	//
private:
	//! Allows to access the member function.
	friend class boost::serialization::access;
	//! Serializes.
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & client_id_;
		ar & session_id_;
		ar & type_;
		ar & has_client_id_;
		ar & has_session_id_;
		ar & has_type_;
	}
	//! Sets has client id.
	void set_has_client_id();
	//! Sets has session id.
	void set_has_session_id();
	//! Sets has type.
	void set_has_type();

	//
	// Private data members.
	//
private:
	//! Client id.
	boost::uuids::uuid client_id_;
	//! Session id.
	boost::uuids::uuid session_id_;
	//! Type.
	protocol::StreamType type_;
	//! Has client id.
	bool has_client_id_;
	//! Has session id.
	bool has_session_id_;
	//! Has type.
	bool has_type_;

};

}
}