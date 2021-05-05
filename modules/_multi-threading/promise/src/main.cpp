#include "pch.h"
#include "App.h"

struct Obj
{
	enum Status
	{
		Stopped,
		NotRunning
	};
	std::promise<Status> p_;
	std::future<void> f_;
	Status status_;
};

void Func(Obj& obj)
{
	std::shared_future<Obj::Status> f(obj.p_.get_future());
	switch (f.wait_for(std::chrono::seconds(10)))
	{
	case std::future_status::ready:
	{
		std::cout << "Ready: ";
		obj.status_ = f.get();
		if (obj.status_ == Obj::Stopped)
			std::cout << "Stopped" << std::endl;
		else if (obj.status_ == Obj::NotRunning)
			std::cout << "Not running" << std::endl;
		break;
	}
	case std::future_status::timeout:
	{
		std::cout << "Time out signaled" << std::endl;
		break;
	}
	case std::future_status::deferred:
	{
		std::cout << "Deferred" << std::endl;
	}
	default:
		break;
	}
}

int main()
{
	try
	{
		Obj obj;
		obj.f_ = std::async(std::launch::async, std::bind(Func, std::ref(obj)));
		std::this_thread::sleep_for(std::chrono::seconds(2));
		obj.p_.set_value(Obj::Stopped);

		App app;
		app.WaitForTerminationRequest();
	}
	catch (std::exception const& e)
	{
		std::cerr << "Error occurred: " << e.what();
	}

	return 0;
}