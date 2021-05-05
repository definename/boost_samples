#include "pch.h"

static const int MAX_SIZE = 10;

void Func(boost::optional<std::string> arr);
int main(int argc, char* argv[])
{
    char arr[MAX_SIZE];
    strcpy(arr, "data");

    std::cout << "origin: " << arr << std::endl;
    Func(boost::make_optional<std::string>(arr));

    {
        char arr[MAX_SIZE];
        std::string data("123321");
//         data.copy(arr, data.size());
//         arr[data.size()] = '\0';
//         std::cout << arr << std::endl;
    }

    {
        boost::optional<int> val;
        val = 10;
        val = boost::none;
        if (val)
            std::cout << *val << std::endl;
    }

    return 0;
}

void Func(boost::optional<std::string> arr)
{
    boost::optional<std::string> internal;
    internal = arr;
    std::cout << "copy: " << *internal << std::endl;
}