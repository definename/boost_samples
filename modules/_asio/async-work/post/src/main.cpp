#include "pch.h"

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

void Func(int i, boost::mutex& mutex) {
	boost::mutex::scoped_lock l(mutex);
	std::cout << i << std::endl;
}

int main()
{
	boost::mutex mutex;
	boost::asio::io_service io;
	for (int i = 0; i != 10; ++i) {
		io.post(boost::bind(Func, i, boost::ref(mutex)));
	}

	std::vector<boost::thread> vec;
	for (int i = 0; i != 2; ++i) {
		vec.emplace_back(boost::bind(Worker, boost::ref(io)));
	}

	for (auto& val : vec) {
		val.join();
	}

	return 0;
}