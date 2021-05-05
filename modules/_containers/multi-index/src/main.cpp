#include "pch.h"

struct Employee
{
	Employee(int id, const std::string& name, int ssnumber)
		: id(id), name(name), ssnumber(ssnumber)
	{ }

	bool operator<(const Employee& e)const { return id < e.id; }

	int id;
	std::string name;
	int ssnumber;
};

typedef boost::multi_index::multi_index_container<
	Employee,
	boost::multi_index::indexed_by<
	// sort by employee::operator<
	boost::multi_index::ordered_unique<boost::multi_index::identity<Employee> >,

	// sort by less<string> on name
	boost::multi_index::ordered_non_unique<boost::multi_index::member<Employee, std::string, &Employee::name> >,

	// sort by less<int> on ssnumber
	boost::multi_index::ordered_unique<boost::multi_index::member<Employee, int, &Employee::ssnumber> >
	>
> employee_set;


int main()
{
	try
	{
		employee_set set;
		Employee empl1(1, "Oleg", 123);
		Employee empl2(2, "Vasya", 321);
		Employee empl3(2, "Vasya", 321);
		
		if (!set.emplace(empl1).second)
			std::cerr << "Unable to insert 1" << std::endl;
		if (!set.emplace(empl2).second)
			std::cerr << "Unable to insert 2" << std::endl;
		if (!set.emplace(empl3).second)
			std::cerr << "Unable to insert 3" << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}