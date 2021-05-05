#pragma once

namespace proxy
{
namespace protocol
{

//! Forward declaration.
class Login;
class Greeting;

class Event
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Event();
	//! Constructor.
	Event(const Greeting& greeting, const Login& login);
	//! Destructor.
	~Event();

	//
	// Public interface.
	//
public:
	//! Returns greeting.
	const Greeting& greeting();
	//! Returns login.
	const Login& login();
	//! Sets login.
	void set_login(const Login& login);
	//! Sets greeting.
	void set_greeting(const Greeting& greeting);
	//! Has login.
	bool has_login() const;
	//! Has greeting.
	bool has_greeting() const;

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
		ar & login_;
		ar & greeting_;
		ar & has_login_;
		ar & has_greeting_;
	}
	//! Sets has login.
	void set_has_login();
	//! Sets has greeting.
	void set_has_greeting();

	//
	// Private data members.
	//
private:
	//! Greeting.
	Greeting greeting_;
	//! Login.
	Login login_;
	//! Has greeting.
	bool has_greeting_;
	//! Has login.
	bool has_login_;
};

}
}