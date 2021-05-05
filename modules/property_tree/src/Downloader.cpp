#include "pch.h"
#include "Downloader.h"

Downloader::Downloader(
	boost::asio::io_service& ioService,
	const std::string& server,
	const std::string& port,
	const std::string& path,
	const boost::filesystem::path& destination)
	: resolver_(ioService)
	, socket_(ioService)
{
	std::ostream requestStream(&request_);
	requestStream << "GET " << path << " HTTP/1.1\r\n";
	requestStream << "Host: " << server << "\r\n";
	requestStream << "Accept: */*\r\n";
	requestStream << "Connection: close\r\n\r\n";

	asio::ip::tcp::resolver::query query(asio::ip::tcp::v4(), server, port);
	resolver_.async_resolve(query,
		boost::bind(&Downloader::HandleResolve, this,
		asio::placeholders::error,
		asio::placeholders::iterator));

	destinationFile_.open(destination.c_str(), std::ios::binary);
	if (!destinationFile_.is_open())
		throw std::runtime_error("Unable to create file");
}

Downloader::~Downloader()
{
	destinationFile_.close();
}

void Downloader::HandleResolve(
	const boost::system::error_code& err, asio::ip::tcp::resolver::iterator endpointIterator)
{
	if (!err)
	{
		// Attempt a connection to each endpoint in the list until we
		// successfully establish a connection.
		asio::async_connect(socket_, endpointIterator,
			boost::bind(&Downloader::HandleConnect, this, asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void Downloader::HandleConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		// The connection was successful. Send the request.
		asio::async_write(socket_, request_,
			boost::bind(&Downloader::HandleWriteRequest, this,
			asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void Downloader::HandleWriteRequest(const boost::system::error_code& err)
{
	if (!err)
	{
		// Read the response status line. The response_ streambuf will
		// automatically grow to accommodate the entire line. The growth may be
		// limited by passing a maximum size to the streambuf constructor.
		asio::async_read_until(socket_, response_, "\r\n",
			boost::bind(&Downloader::HandleReadStatusLine, this, asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err.message() << "\n";
	}
}

void Downloader::HandleReadStatusLine(const boost::system::error_code& err)
{
	if (!err)
	{
		// Check that response is OK.
		std::istream response_stream(&response_);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			std::cout << "Invalid response\n";
			return;
		}
		if (status_code != 200)
		{
			std::cout << "Response returned with status code ";
			std::cout << status_code << "\n";
			return;
		}

		// Read the response headers, which are terminated by a blank line.
		asio::async_read_until(socket_, response_, "\r\n\r\n",
			boost::bind(&Downloader::HandleReadHeaders, this, asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err << "\n";
	}
}

void Downloader::HandleReadHeaders(const boost::system::error_code& err)
{
	if (!err)
	{
		// Process the response headers.
		std::istream response_stream(&response_);
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
			std::cout << header << "\n";
		std::cout << "\n";

		// Write whatever content we already have to output.
		if (response_.size() > 0)
		{
			//std::cout << &response_;
			destinationFile_ << &response_;
		}

		// Start reading remaining data until EOF.
		boost::asio::async_read(socket_, response_,
			asio::transfer_at_least(1),
			boost::bind(&Downloader::HandleReadContent, this, asio::placeholders::error));
	}
	else
	{
		std::cout << "Error: " << err << "\n";
	}
}

void Downloader::HandleReadContent(const boost::system::error_code& err)
{
	if (!err)
	{
		// Write all of the data that has been read so far.
		std::cout << "\n-------------------------" << response_.size() << "\n";
		//std::cout << &response_;
		destinationFile_ << &response_;

		// Continue reading remaining data until EOF.
		asio::async_read(socket_, response_,
			asio::transfer_at_least(1), 
			boost::bind(&Downloader::HandleReadContent, this, asio::placeholders::error));
	}
	else if (err != asio::error::eof)
	{
		std::cout << "Error: " << err << "\n";
	}
}