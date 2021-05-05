#include "pch.h"

int main()
{
	try
	{
		boost::asio::io_context ioc;
		boost::asio::ip::tcp::resolver resolver{ioc};
		boost::asio::ip::tcp::socket socket(ioc);

		std::string host("reports.adexpertsmedia.com");
		std::string target("/rest/trackinstall/restore/?advId=23&offerId=14&campaignId=92&ip=183.82.194.181&timestamp=1520418292702&key=WaxUjIYMxH3sTBNRjEMGcmWOWPMuyzAF");

		boost::system::error_code ec;
		auto endpoint = resolver.resolve(host, "http", ec);
		if (ec) {
			throw std::runtime_error("Unable to resolve given endpoint: " + ec.message());
		}

		boost::asio::connect(socket, endpoint.begin(), endpoint.end(), ec);
		if (ec) {
			throw std::runtime_error("Unable to connect to given endpoint: " + ec.message());
		}

		boost::beast::http::request<boost::beast::http::empty_body> req;
		req.version(11);
		req.method(boost::beast::http::verb::get);
		req.target(target);
		req.set(boost::beast::http::field::host, host);
		req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

		boost::beast::http::write(socket, req, ec);
		if (ec) {
			throw std::runtime_error("Unable to write request: " + ec.message());
		}

		boost::beast::flat_buffer buff;
		boost::beast::http::response<boost::beast::http::dynamic_body> res;
		boost::beast::http::read(socket, buff, res, ec);
		if (ec) {
			throw std::runtime_error("Unable to read response: " + ec.message());
		}

		if (res.result() == boost::beast::http::status::ok) {
			std::cout << res << std::endl;
		}

		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec && ec != boost::system::errc::not_connected) {
			throw boost::system::system_error{ ec };
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}