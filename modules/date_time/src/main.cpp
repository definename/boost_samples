#include "pch.h"

inline uint64_t timestamp()
{
	FILETIME object_filetime;
	ULARGE_INTEGER object_time;
	GetSystemTimeAsFileTime(&object_filetime);
	object_time.LowPart = object_filetime.dwLowDateTime;
	object_time.HighPart = object_filetime.dwHighDateTime;

	return object_time.QuadPart;
}

void LocalToStrToTimeT()
{
    auto time = boost::posix_time::second_clock::local_time();
    std::string str = boost::posix_time::to_iso_string(time);
    std::time_t tt = boost::posix_time::to_time_t(boost::posix_time::from_iso_string(str));
}

int main()
{
	std::cout << timestamp() << std::endl;

    LocalToStrToTimeT();

//     boost::gregorian::date date(2014,12,24);
//     boost::posix_time::time_duration duration(10,20,30);
// 
//     boost::posix_time::ptime time1(date, duration), time2;
// 
//     time2 = boost::posix_time::second_clock::local_time();
// 
//     //! Posix time.
//     std::cout << "Posix time simple string: " << boost::posix_time::to_simple_string(time2) << std::endl;
//     std::cout << "Posix time iso string: " << boost::posix_time::to_iso_string(time2) << std::endl;
//     std::cout << time2.date() << std::endl;
//     std::cout << time2.time_of_day() << std::endl;
// 
//     //! Time duration.
//     boost::posix_time::time_duration td = boost::posix_time::seconds(10);
//     std::cout << "Time duration from posix seconds: " << td << std::endl;
//     boost::posix_time::time_duration dTime = time2 - time1;
//     std::cout << "Time duration simple string: " << boost::posix_time::to_simple_string(dTime) << std::endl;
//     std::cout << "Time duration iso string: " << boost::posix_time::to_iso_string(dTime) << std::endl;
// 	
//     //! Time period.
//     boost::posix_time::time_period pTime(time1, time2);
//     std::cout << "Time period: " << boost::posix_time::to_simple_string(pTime) << std::endl;
//     std::cout << "Time period length: " << pTime.length() << std::endl;
// 
//     //! Ptime from isoDate format
//     std::string timeStr("2012-03-28T08:08:08");
// 
//     boost::posix_time::ptime ship(boost::date_time::parse_delimited_time<boost::posix_time::ptime>(timeStr, 'T'));
//     std::cout << "Date: " << ship.date() << std::endl;
//     std::cout << "Time: " << ship.time_of_day() << std::endl;
// 
//     return 0;
}