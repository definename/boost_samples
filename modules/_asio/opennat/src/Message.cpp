#include "pch.h"
#include "Command.pb.h"
#include "nat/Message.h"
#include "nat/Utils.h"

namespace opennat
{

Message::Message()
{ }

Message::~Message()
{
	try
	{
		types::Buff().swap(buffer_);
	}
	catch (const std::exception& e)
	{
		LERR_ << "Message destruction error: " << e.what();
	}
}

types::Buff& Message::GetBuff()
{
	return buffer_;
}

types::Buff::value_type* Message::GetData()
{
	return buffer_.data();
}

const size_t Message::GetSize() const
{
	return buffer_.size();
}

}