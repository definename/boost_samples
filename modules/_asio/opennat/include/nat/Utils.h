#pragma once

#include "nat/Types.h"

namespace opennat
{
namespace utils
{

//! Stream to buffer.
void StreamToBuff(types::StreamBuff& stream, types::Buff& buff, const size_t offset);
//! Copies buffer to another buffer.
//size_t CopyBuffer(types::Buff& target, types::Buff& source, size_t size);

//! Packs header into 8 byte array.
types::Header Pack(const uint64_t& data);
//! Unpacks 8 byte array into header.
uint64_t Unpack(const types::Header& bytes);

}
}