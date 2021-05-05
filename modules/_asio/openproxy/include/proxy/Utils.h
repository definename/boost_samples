#pragma once

#include "proxy/Types.h"

namespace proxy
{
namespace utils
{

//! Writes asio streambuf to stream.
void BufferToSteam(boost::asio::streambuf& buf, std::stringstream& stream, const size_t offset);
//! Copies buffer to another buffer.
size_t CopyBuffer(types::Buff& target, types::Buff& source, size_t size);

//! Serializes.
template <typename TOArch, typename TSource, typename TTarget>
void SerializeTo(const TSource& source, TTarget& target)
{
	TOArch oa(target);
	oa << source;
}
//! Serializes to asio streambuf.
template<typename TSource>
void SerializeToBuffer(const TSource& source, boost::asio::streambuf& target)
{
	std::ostream stream(&target);
	SerializeTo<boost::archive::text_oarchive>(source, stream);
	stream.write(protocol::delimiter.c_str(), protocol::delimiter.size() * sizeof(char));
}

//! Deserializes.
template<typename TIArch, typename TSource, typename TTarget>
void DeserializeFrom(TSource& source, TTarget& target)
{
	TIArch ia(source);
	ia >> target;
}
//! Deserializes from asio buffer.
template<typename TTarget>
void DeserializeFromBuffer(boost::asio::streambuf& source, TTarget& target, const size_t bytes)
{
	std::stringstream stream;
	utils::BufferToSteam(source, stream, bytes);
	DeserializeFrom<boost::archive::text_iarchive>(stream, target);
}

}
}