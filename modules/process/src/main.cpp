#include "pch.h"

int main()
{
	//D:\>msiexec /i d:\pci-inventory.msi
//	auto p = boost::process::search_path("msiexec");
// 	std::string str(p.string());
// 	str.append(" /i d:\\pci-inventory.msi /qn");
// 
// 	std::error_code ec;
// 	boost::process::child c(str, ec);
// 	if (ec)
// 		std::cout << ec.message() << std::endl;
// 
// 	c.wait_for(std::chrono::seconds(10));
// 	if (c.running()) c.terminate(ec);
//	return c.exit_code();

	boost::filesystem::path p("d://pci-inventory.msi");
	std::error_code ec;
	boost::process::system(p, "/qb", ec, boost::process::shell);
	if (ec)
		std::cout << ec.message() << std::endl;

	return 0;
}