#include "pch.h"

//! SHA1 generator.
auto GenerateSha1(const std::string& data)
{
	std::string a;
	{
		boost::compute::detail::sha1 sha1(data);
		a = sha1;
	}

	std::string b;
	{
		boost::uuids::detail::sha1 sha;
		sha.process_bytes(data.c_str(), data.size());
		boost::uuids::detail::sha1::digest_type dig = {};
		sha.get_digest(dig);

		b = boost::str(boost::format("%08x%08x%08x%08x%08x")
			% dig[0]
			% dig[1]
			% dig[2]
			% dig[3]
			% dig[4]
		);
	}
	return std::make_tuple(a, b);
}

//! Generates uuid based on input string.
boost::uuids::uuid GenerateUuid1(const std::string& input);
//! Generates uuid based on input string.
boost::uuids::uuid GenerateUuid2(const std::string& input);
//! Generates uuid based on input string.
boost::uuids::uuid GenerateUuid3(const std::string& input);

int main(int argc, char* argv[])
{
	try
	{
		const std::string input("b888e35f9edf3794760392e1066d69-f43d-452e-8475-a09bae9a2e8500000000-0000-0000-0000-000000000000");

		const auto[a, b] = GenerateSha1(input);
		std::cout << a << std::endl;
		std::cout << b << std::endl;

		std::cout << GenerateUuid1(input) << std::endl;
		std::cout << GenerateUuid2(input) << std::endl;
		std::cout << GenerateUuid3(input) << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << "Error occurred: " << e.what() << std::endl;
	}

	return 0;
}

boost::uuids::uuid GenerateUuid1(const std::string& input)
{
	boost::uuids::detail::sha1 sha;
	sha.process_bytes(input.data(), input.size());
	boost::uuids::detail::sha1::digest_type dig = {};
	sha.get_digest(dig);

	std::ostringstream ss;
	std::for_each(std::begin(dig), std::begin(dig) + 4, [&ss](const unsigned int dig) {
		ss << std::hex << std::setfill('0') << std::setw(8) << dig;
	});
	return boost::uuids::string_generator{}(ss.str());
}

boost::uuids::uuid GenerateUuid2(const std::string& input)
{
	auto rootns = boost::uuids::string_generator{}("bd89d3ca-8776-11e8-9a94-a6cf71072f73");
	return boost::uuids::name_generator_sha1{rootns}(input.data(), input.size());
}

boost::uuids::uuid GenerateUuid3(const std::string& input)
{
	boost::uuids::detail::sha1::digest_type digest;
	{
		boost::uuids::detail::sha1 h;
		h.process_bytes(input.data(), input.size());
		h.get_digest(digest);
	}

	boost::uuids::uuid ret;
	auto p = ret.begin();
	for (std::size_t i = 0; i != 4; p += 4, ++i)
	{
		auto const d = boost::endian::native_to_big(digest[i]);
		std::memcpy(p, &d, sizeof(d));
	}
	return ret;
}