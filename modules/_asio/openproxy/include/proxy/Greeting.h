#pragma once

namespace proxy
{
namespace protocol
{

class Greeting
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Greeting();
	//! Destructor.
	~Greeting();

	//
	// Public interface.
	//
public:
	//! Returns ready.
	bool ready();
	//! Sets ready.
	void set_ready();
	//! Has ready.
	bool has_ready() const;

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
		ar & ready_;
		ar & has_ready_;
	}
	//! Sets has ready.
	void set_has_ready();

	//
	// Private data members.
	//
private:
	//! Ready.
	bool ready_;
	//! Has ready.
	bool has_ready_;
};

}
}