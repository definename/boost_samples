#include "pch.h"

namespace algo
{
	typedef std::vector<std::string> Out;
	void Split();
	void Replace();
	void Contains();
}

int main()
{
	algo::Split();
	algo::Replace();
	algo::Contains();

	return 0;
}

void algo::Split()
{
	std::string in = "one two three w www qwe wwwe wewe wwwe rrg ddds qwww sjh www";
	Out out;
	std::string token("");
	boost::algorithm::split(out, in, boost::algorithm::is_any_of(token), boost::token_compress_on);

	for (const Out::value_type& val : out)
	{
		if (val[0] == 'w') std::cout << val << std::endl;
	}
}

void algo::Replace()
{
	std::string seq("1A 1a 1A 1a 1A 1a");
	std::cout << "in: " << seq << std::endl;

	boost::algorithm::replace_all(seq, "1", "_");
	std::cout << "out 'replace_all': "
		<< seq << std::endl;
	std::cout << "out 'replace_all_copy': "
		<< boost::algorithm::replace_all_copy(seq, "a", "_") << std::endl;
	std::cout << "out 'ireplace_all_copy': "
		<< boost::algorithm::ireplace_all_copy(seq, "a", "_") << std::endl;
}

void algo::Contains()
{
	std::string data("Google Chrome");
	if (boost::algorithm::icontains(data, "chrome"))
		std::cout << "Contains" << std::endl;
}