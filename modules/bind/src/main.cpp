#include "pch.h"


/*/////////////////////////////////////////////////////////////////////////
	Represents functionality to Binding an overloaded function
*//////////////////////////////////////////////////////////////////////////


class a
{
public:
	a() {}
	virtual ~a() {}
	void foo(const std::string &str)
	{
		std::cout << "string func: " << str << std::endl;
	}
	void foo(const char *ptr)
	{
		std::cout << "char func: " << ptr << std::endl;
	}
};

class b : public a
{
public:
	b() {}
	~b() {}
};


void FreeFunc1(const int one, const int two);
using FreeFunc1Handler = std::function<void(const int)>;
void FreeFunc2(const FreeFunc1Handler& handler);

int main()
{
	b bb;
	boost::shared_future<void> f;
	////////////////////////////////////////////////////////////////////////// First way.
	//! String member function.
	f = boost::async(boost::bind(
		static_cast<void (b::*) (const std::string&)> (&b::foo), &bb, "str"));
	f.get();

	//! Char member function.
	f = boost::async(boost::bind(
		static_cast<void (b::*) (const char*)> (&b::foo), &bb, "char"));
	f.get();

	////////////////////////////////////////////////////////////////////////// Second way.
	//! String member function.
	std::string str("Lyambda");
	f = boost::async([&bb, &str] () -> void { bb.foo(str); });
	f.get();

	//! Char member function.
	f = boost::async([&bb, &str] () -> void { bb.foo(str.c_str()); });
	f.get();

	FreeFunc2(boost::bind(&FreeFunc1, _1, -1));

	return 0;
}

void FreeFunc1(const int one, const int two)
{
	std::cout << one << " " << two << std::endl;
}

void FreeFunc2(const FreeFunc1Handler& handler)
{
	handler(1);
}