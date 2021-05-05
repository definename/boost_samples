#include "pch.h"


#include <iostream>
#include "boost/logic/tribool.hpp"

int main()
{
	boost::logic::tribool init;
	//init = boost::logic::indeterminate;
	init = true;
	//init = false;

	if (boost::logic::indeterminate(init))
	{
		std::cout << "indeterminate -> " << std::boolalpha << init << std::endl;
	}
	else if (init)
	{
		std::cout << "true -> " << std::boolalpha << init << std::endl;
	}
	else if (!init)
	{
		std::cout << "false -> " << std::boolalpha << init << std::endl;
	}

	return 0;
}