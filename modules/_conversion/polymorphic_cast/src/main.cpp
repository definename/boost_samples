#include "pch.h"

namespace cast
{

class Base
{
public:
	Base()
		: val_(1)
	{ }
	virtual void kind() { std::cout << "base" << std::endl; }
	int val_;
};

class Derived : public Base
{
public:
	Derived()
		: val2_(2)
	{ }
	virtual void kind2() { std::cout << "derived" << std::endl; }
	int val2_;
};

}

int main(int argc, char* argv[])
{
	try
	{
		boost::shared_ptr<cast::Base> base(new cast::Derived());
		base->kind();
		std::cout << base->val_ << std::endl;

		boost::shared_ptr<cast::Derived> d1 = boost::polymorphic_pointer_downcast<cast::Derived>(base);
		d1->kind();
		d1->kind2();
		std::cout << d1->val_ << std::endl;
		std::cout << d1->val2_ << std::endl;

		boost::shared_ptr<cast::Derived> d2 = boost::dynamic_pointer_cast<cast::Derived>(base);
		d2->kind();
		d2->kind2();
		std::cout << d2->val_ << std::endl;
		std::cout << d2->val2_ << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}