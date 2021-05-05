#pragma once

class Character
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Character()
		: power_{0} {
	}
	//! Constructor.
	Character(unsigned int power)
		: power_{power} {
	}
	//! Destructor.
	virtual ~Character() {
	}

	//
	// Public interface.
	//
public:
	//! Returns power.
	virtual unsigned int GetPower() const {
		return power_;
	}
	//! Returns name.
	virtual std::string GetName() const = 0;

	//
	// Private interface.
	//
private:
	//! Friend class.
	friend boost::serialization::access;
	//! Serialization.
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & power_;
	}

	//
	// Private data members.
	//
private:
	//! Power.
	unsigned int power_;
};

BOOST_SERIALIZATION_ASSUME_ABSTRACT(Character)
BOOST_CLASS_VERSION(Character, 1)