#include "pch.h"

int main()
{
	try
	{
		std::ofstream file("lib.txt", std::ios::out);
		if (!file)
			throw std::runtime_error("Unable to open destination file");

		boost::filesystem::path dir(boost::filesystem::current_path());
		file << "Scan path: " << dir.string() << std::endl;

		boost::filesystem::directory_iterator it(dir);
		for (it; it != boost::filesystem::end(it); it++)
		{
			if (it->status().type() == boost::filesystem::regular_file)
			{
				if (boost::iequals(it->path().filename().extension().string(), ".lib"))
				{
					std::string filename = it->path().filename().leaf().string();
					file << filename << std::endl;
					std::cout << filename << std::endl;
				}
			}
		}
		std::cout << "...done!!";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}
	std::cin.get();

	return 0;
}