#include "pch.h"

//! User credentials.
typedef std::pair<std::string, std::string> UserCredentials;
//! User credentials pointer.
typedef std::shared_ptr<UserCredentials> UserCredentialsPtr;

void Impersonation(UserCredentialsPtr credentials);

int main()
{
	try
	{
		UserCredentialsPtr credentials(new UserCredentials("inlineortho\\activeitek", "allthetime1!"));
		Impersonation(credentials);
	}
	catch (std::exception& e)
	{
		_tcerr << "Error occurred: " << e.what() << std::endl;

		_twostringstream error;
		error << _T("Error: ") << e.what() << _T("\nUsage: resolver.exe oleg-pc");
		::MessageBox(NULL, error.str().c_str(), _T("Error occurred"), MB_ICONERROR);
	}

	return 0;
}

void Impersonation(UserCredentialsPtr credentials)
{
	std::pair<std::string, std::string> out;
	bool res = boost::spirit::qi::phrase_parse(
		credentials->first.begin(),
		credentials->first.end(),
		+(boost::spirit::qi::char_ - boost::spirit::qi::char_("\\/")) >> -(boost::spirit::qi::lit("\\") | boost::spirit::qi::lit("/")) >> *(boost::spirit::qi::char_),
		boost::spirit::qi::space,
		out);

	std::cout << "Login: " << credentials->first << std::endl;
	std::cout << "Password: " << credentials->second << std::endl;

	if (out.second.empty())
	{
		_tcout << _T("Logon with out domain") << std::endl;
		_tcout << _T("User name: ") << CU2CW(out.first) << std::endl;
	}
	else
	{
		_tcout << _T("Logon with domain") << std::endl;
		_tcout << _T("Domain: ") << CU2CW(out.first) << std::endl;
		_tcout << _T("User name: ") << CU2CW(out.second) << std::endl;
	}
}