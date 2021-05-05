#pragma once
#include "Character.h"

class DarthVader : public Character
{
	//
	// Construction and destruction.
	//
public:
	DarthVader()
		: name_("Darth") {
	}
	//! Constructor.
	DarthVader(std::string name, unsigned int power)
		: Character(power)
		, name_(name) {
	}
	//! Destructor.
	virtual ~DarthVader() {
	}

	//
	// Public interface.
	//
public:
	//! Returns name.
	std::string GetName() const {
		return name_;
	}

	//
	// Private interface.
	//
private:
	//! Friend class.
	friend boost::serialization::access;
	//! Serialization.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & boost::serialization::base_object<Character>(*this);
		ar & name_;
	}

	//
	// Private data members.
	//
private:
	//! Name.
	std::string name_;
};

BOOST_CLASS_VERSION(DarthVader, 1)
BOOST_CLASS_EXPORT(DarthVader)