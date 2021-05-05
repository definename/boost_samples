#include "pch.h"

// The io_service::strand class provides the ability to post and dispatch handlers 
// with the guarantee that none of those handlers will execute concurrently.

void Worker(boost::asio::io_service& io) {
	try {
		boost::system::error_code ec;
		io.run(ec);
		if (ec)
			std::cerr << "Unable to run io service: " << ec.message() << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << "Worker error: " << e.what() << std::endl;
	}
}

void Func(int i) {
	std::cout << i << " ";
}

int main()
{
	for (int y = 0; y != 20; ++y)
	{
		boost::asio::io_service io;
		boost::asio::io_service::strand strand(io);

		for (int i = 0; i != 20; ++i) {
			//io.post(strand.wrap(boost::bind(Func, i))); // does not give any guarantee that the jobs are executed in the same order they were posted
			strand.post(boost::bind(Func, i)); // will insure that the jobs are executed in the same order they were posted
		}

		std::vector<boost::thread> vec;
		for (int i = 0; i != 10; ++i) {
			vec.emplace_back(boost::bind(Worker, boost::ref(io)));
		}

		for (auto& val : vec) {
			val.join();
		}
		std::cout << std::endl;
	}

	return 0;
}