#pragma once

class App
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	App()
		: terminated_(false)
	{ }
	//! Destructor.
	virtual ~App()
	{
		Terminate();
	}

	//
	// Public interface
	//
public:
	//! Terminate.
	virtual void Terminate()
	{
		std::unique_lock<std::mutex> l(appMutex_);
		terminated_ = true;
		appCond_.notify_all();
	}
	//! Waits for termination request.
	void WaitForTerminationRequest()
	{
		std::unique_lock<std::mutex> l(appMutex_);
		while (!terminated_)
			appCond_.wait(l);
	}

	//
	// Private data members.
	//
private:
	//! App condition variable.
	std::condition_variable_any appCond_;
	//! App synchronization mutex.
	std::mutex appMutex_;
	//! Termination flag.
	bool terminated_;
};