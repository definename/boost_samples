class MyTimer {
public:
	MyTimer(boost::asio::io_context& ctx)
		: timer_(ctx) {
	}	
	~MyTimer() {
		try {
			timer_.cancel();
		} catch (const std::exception& e) {
			std::cout << "Timer destruction error: " << e.what() << std::endl;
		}
	}

public:
	void Run() {
		timer_.expires_after(std::chrono::seconds(1));
		timer_.async_wait(std::bind(&MyTimer::TimeoutHandler, this, std::placeholders::_1));
	} 

private:
	void TimeoutHandler(const boost::system::error_code& ec) {
		if (ec == boost::asio::error::operation_aborted) {
			std::cout << "aborted" << std::endl;
			return;
		}

		static int count = 0;
		if (timer_.expiry() < boost::asio::steady_timer::clock_type::now()) {
			std::cout << "expired" << count << std::endl;
		}

		if (count < 5) {
			std::cout << "run one more time" << std::endl;
			timer_.expires_after(std::chrono::seconds(1));
			timer_.async_wait(std::bind(&MyTimer::TimeoutHandler, this, std::placeholders::_1));
			++count;
		} else {
			std::cout << "timer exit" << std::endl;
		}
	}
 
private:
	boost::asio::steady_timer timer_;
};

int main(int argc, char* argv[])
{
	try {
		boost::asio::io_context ctx;
		// in order not to exit when there is no more work to do.
		boost::asio::executor_work_guard<boost::asio::io_context::executor_type> work = boost::asio::make_work_guard(ctx);

		MyTimer timer(ctx);
		timer.Run();
		ctx.run();

	} catch (std::exception& e) {
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}
	return 0;
}