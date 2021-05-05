#pragma once

class Test
{
	//! Allow to access the member function.
	friend boost::serialization::access;
	//! Saves archive.
	template<class Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		ar & data_;
		ar & extraData_;
	}
	//! Loads archive.
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		ar & data_;
		if (version > 0) {
			ar & extraData_;
		}
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()

	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Test()
		: data_{0}
		, extraData_{ false }
	{
	}
	//! Constructor.
	Test(unsigned int data, bool extraData)
		: data_{ data }
		, extraData_{ extraData } {
	}
	//! Destructor.
	~Test() {
	}

	//
	// Public interface.
	//
public:
	//! Returns data.
	unsigned int GetData() const {
		return data_;
	}
	//! Returns extra data.
	bool GetExtra() const {
		return extraData_;
	}

	//
	// Private data members.
	//
private:
	//! Data.
	unsigned int data_;
	//! Extra data.
	bool extraData_;
};

BOOST_CLASS_VERSION(Test, 1)