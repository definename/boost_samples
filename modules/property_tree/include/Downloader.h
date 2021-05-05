#pragma once

class Downloader
{
public:
	//! Constructor.
	Downloader(
		asio::io_service& ioService,
		const std::string& server,
		const std::string& port,
		const std::string& path,
		const fs::path& destination);
	//! Destructor.
	~Downloader();

	//
	// Private interface.
	//
private:
	//! Handles resolve.
	void HandleResolve(
		const boost::system::error_code& err, asio::ip::tcp::resolver::iterator endpointIterator);
	//! Handles connect.
	void HandleConnect(const boost::system::error_code& err);
	//! Handles write request.
	void HandleWriteRequest(const boost::system::error_code& err);
	//! Handles status line.
	void HandleReadStatusLine(const boost::system::error_code& err);
	//! Handles headers.
	void HandleReadHeaders(const boost::system::error_code& err);
	//! Handles content.
	void HandleReadContent(const boost::system::error_code& err);

	//
	// Private data members.
	//
private:
	//! TCP resolver.
	asio::ip::tcp::resolver resolver_;
	//! Socket.
	asio::ip::tcp::socket socket_;
	//! Request buffer.
	boost::asio::streambuf request_;
	//! REsponse buffer.
	boost::asio::streambuf response_;
	//! Destination file.
	std::ofstream destinationFile_;
};