#include "pch.h"
#include "proxy/Types.h"
#include "proxy/Utils.h"

namespace proxy
{
namespace utils
{

void BufferToSteam(boost::asio::streambuf& buf, std::stringstream& stream, const size_t offset)
{
	std::copy(
		boost::asio::buffers_begin(buf.data()),
		boost::asio::buffers_begin(buf.data()) + offset - protocol::delimiter.size(),
		std::ostream_iterator<char>(stream));
}

size_t CopyBuffer(types::Buff& target, types::Buff& source, size_t size)
{
	size_t copied = boost::asio::buffer_copy(target.prepare(size), source.data());
	target.commit(copied);

	return copied;
}

}
}