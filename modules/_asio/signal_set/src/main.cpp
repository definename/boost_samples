#include "pch.h"

void SignalHandler(const boost::system::error_code& ec, int signal);

int main()
{
	boost::asio::io_service io;
	boost::asio::signal_set sig(io, SIGINT);
	sig.async_wait(boost::bind(SignalHandler, boost::asio::placeholders::error, _2));
	io.run(); 

	return 0;
}

void SignalHandler(const boost::system::error_code& ec, int signal)
{
	std::cout << "Invoked" << std::endl;
}