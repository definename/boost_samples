#pragma once

class Monitor
{
	//! Construction and destruction.
public:
	//! Constructor.
	Monitor();
	//! Destructor.
	~Monitor();

	//! Public interface.
public:
	//! Starts monitoring.
	void StartMonitor();
	//! Stops monitor.
	void Stop();
	//! Returns is running flag.
	bool IsRunning() const;

	//! Private members.
private:
	//! Is monitor running.
	volatile bool isRunning_;
};