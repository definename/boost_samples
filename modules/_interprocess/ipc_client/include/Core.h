#pragma once

namespace ipc
{

class Core
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	explicit Core::Core(const unsigned int workers = 1);
	//! Destructor.
	virtual ~Core();

	//
	// Protected interface.
	//
protected:
	//! Starts.
	void Run();
	//! Stops.
	void Stop();
	//! Is running
	bool IsRunning() const;

	//
	// Protected data members.
	//
protected:
	//! IO service.
	boost::shared_ptr<boost::asio::io_service> ioService_;
	//! IO work.
	boost::shared_ptr<boost::asio::io_service::work> ioWork_;
	//! IO strand.
	boost::shared_ptr<boost::asio::io_service::strand> ioStrand_;
	//! IO threads.
	std::vector<boost::thread> threads_;
	//! Workers count.
	const unsigned int workers_;
	//! Is running.
	bool isRunning_;
};

}