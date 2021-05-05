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

void Func(int i) {
	std::cout << i << std::endl;
}

void RunPostAndDispatch(boost::asio::io_service& io) {
	for (int i = 0; i != 10; ++i) {
		io.dispatch(boost::bind(Func, i * 10));
		io.post(boost::bind(Func, i));
	}
}

int main()
{
	boost::asio::io_service io;
	io.post(boost::bind(RunPostAndDispatch, boost::ref(io)));
	Worker(io);

	return 0;
}