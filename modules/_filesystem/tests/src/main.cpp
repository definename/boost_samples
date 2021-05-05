#include "pch.h"

//! Represent functionality to test remove function.
void TestRemoveDirectory();
//! Represent functionality to test create_directory function.
void TestCreateDirectory();
//! Tests remove dir files.
void TestRemoveDirFiles(const boost::filesystem::path& dir);

void _tmain(int argc, TCHAR* argv[])
{
	try
	{
		setlocale(LC_ALL, "");
// 		TestCreateDirectory();
// 		TestRemoveDirectory();
		boost::filesystem::path dir = boost::filesystem::current_path();
		TestRemoveDirFiles(dir);
	}
	catch (const std::exception& e)
	{
		_tcerr << "Error occurred: " << e.what() << std::endl;

		_twostringstream error;
		error << _T("Error: ") << e.what();
		::MessageBox(NULL, error.str().c_str(), _T("Error occurred"), MB_ICONERROR);
	}
}

void TestRemoveDirFiles(const boost::filesystem::path& dir)
{
	try
	{
		std::cout << "Scan path: " << dir.string() << std::endl;

		boost::system::error_code ec;
		boost::filesystem::directory_iterator it(dir, ec);
		if (!ec)
		{
			for (it; it != boost::filesystem::end(it); ++it)
			{
				if (it->status().type() != boost::filesystem::regular_file
					|| !it->path().has_filename()
					|| !it->path().has_extension()
					|| !boost::icontains(it->path().filename().wstring(), L"TrayAgent")
					|| !boost::iequals(it->path().filename().extension().wstring(), L".log")) {
					continue;
				}
				boost::filesystem::remove(*it, ec);
			}
		}
		std::cout << "...done!!";
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}
	std::cin.get();
}

void TestCreateDirectory()
{
	boost::filesystem::path p("D:/Create_directory/Test");
	boost::system::error_code ec;
	boost::filesystem::create_directory(p, ec);
	if (ec) {
		_tcout << _T("create_directory error: ") << ec.value() << std::endl;
	}
}

void TestRemoveDirectory()
{
	boost::filesystem::path p(_T("C:/ProgramData/something.exe"));
	if (!boost::filesystem::remove(p)) {
		throw std::runtime_error("File does not exist");
	}
	_tcout << "File successfully removed" << std::endl;
}