#pragma once

class Runnable
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Runnable(void);
	//! Destructor.
	virtual ~Runnable(void);

	//
	// Public interface.
	//
public:
	//! Checks if is running.
	virtual bool IsRunning(void) const;
	//! Starts service.
	virtual void Run(void);
	//! Stops service.
	virtual void Stop(void);

	//
	// Protected members.
	//
protected:
	//! I/O service.
	boost::asio::io_service ioService_;
	//! Is I/O service running?
	volatile bool isRunning_;
	//! Worker thread.
	boost::thread thread_;
	//! I/O work.
	boost::shared_ptr<boost::asio::io_service::work> work_;
};
