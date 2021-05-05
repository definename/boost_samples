#include "pch.h"

class output : public boost::static_visitor<std::string>
{
public:
    std::string operator()(int i) const {
        std::cout << i << std::endl;
        return "return type";
    }
    std::string operator()(double d) const {
        std::cout << d << std::endl;
        return "return type";
    }
    std::string operator()(const std::string& s) const {
        std::cout << s << std::endl;
        return "return type";
    }
};

int main(int argc, char* argv[])
{
	try
	{
        boost::variant<double, std::string, int> v;
        v = "qwerty";
        std::cout << boost::get<std::string>(v) << std::endl;
        v = 9.0;
        std::cout << v << std::endl;
        v = 99;
        output out;
        std::cout << boost::apply_visitor(out, v) << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}