#include "pch.h"
#include "nat/Utils.h"
#include "Command.pb.h"

namespace opennat
{
namespace utils
{

void StreamToBuff(types::StreamBuff& stream, types::Buff& buff, const size_t offset)
{
	std::copy(
		boost::asio::buffers_begin(stream.data()),
		boost::asio::buffers_begin(stream.data()) + offset - opennat::Protocol().default_instance().delimiter().size(),
		std::back_inserter(buff));
}

// size_t CopyBuffer(types::Buff& target, types::Buff& source, size_t size)
// {
// 	size_t copied = boost::asio::buffer_copy(target.prepare(size), source.data());
// 	target.commit(copied);
// 
// 	return copied;
// }

types::Header Pack(const uint64_t& data)
{
	types::Header bytes;

	std::size_t bits = sizeof(uint64_t) * 8;
	for (std::size_t offset = 0; offset < bits; offset += 8)
	{
		bytes[offset / 8] = (data >> offset) & 0xFF;
	}
	std::reverse(bytes.begin(), bytes.end());

	return bytes;
}

uint64_t Unpack(const types::Header& bytes)
{
	uint64_t header = 0;

	for (int i = 0, offset = (sizeof(uint64_t) - 1) * 8; offset >= 0; ++i, offset -= 8)
	{
		uint64_t byte = static_cast<uint64_t>(bytes.at(i));
		header |= (byte << offset);
	}

	return header;
}

}
}