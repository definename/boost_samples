#include "pch.h"

int main()
{

	try
	{
		typedef std::map<std::string, std::string> Headers;
		Headers headers;

		std::vector<std::string> rawHeaders;
		rawHeaders.push_back("Server: nginx/1.9.6\r");
		rawHeaders.push_back("Date: Fri, 10 Jun 2016 09 : 01 : 28 GMT\r");
		rawHeaders.push_back("Content-Type: application/octet-stream\r");
		rawHeaders.push_back("Content-Length: 8212480\r");
		rawHeaders.push_back("Last-Modified: Mon, 30 May 2016 15 : 06 : 51 GMT\r");
		rawHeaders.push_back("Connection: close\r");
		rawHeaders.push_back("ETag: '574c570b-7d5000'\r");
		rawHeaders.push_back("Accept-Ranges: bytes\r");

		for (const std::string& val : rawHeaders)
		{
			bool res = boost::spirit::qi::phrase_parse(
				val.begin(),
				val.end(),
				+(+(boost::spirit::qi::char_ - ": ") >> ": " >> +(boost::spirit::qi::char_ - "\r")),
				boost::spirit::qi::char_('_'),
				headers);
		}

		Headers::iterator it = headers.find("Content-Length");
		if (it != headers.end())
		{
			std::cout << "Content-Length has been found" << std::endl;
			std::streamsize i = boost::lexical_cast<std::streamsize>(it->second);
			std::cout << it->first << " " << boost::lexical_cast<std::streamsize>(it->second) << std::endl;
		}
		else
		{
			std::cout << "Unable to find Content-Length" << std::endl;
			for (const Headers::value_type& val : headers)
			{
				std::cout << "name: " << val.first << std::endl;
				std::cout << "value: " << val.second << std::endl;
				std::cout << std::endl;
			}
		}

		std::cout << "Bye... :-) \n\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "Error occured: " << e.what() << std::endl;
	}
	return 0;
}