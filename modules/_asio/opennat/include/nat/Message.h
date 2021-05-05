#pragma once

namespace opennat
{

class Message
{
	//
	// Construction and destruction.
	//
public:
	//! Constructor.
	Message();
	//! Destructor.
	~Message();

	//
	// Public interface.
	//
public:
	//! Returns a direct pointer to the memory array used internally by the message.
	types::Buff::value_type* GetData();
	//! Returns a reference to the array used internally by the message.
	types::Buff& GetBuff();
	//! Returns message size.
	const size_t GetSize() const;
	//! Sets message buffer.
	template <typename TMessage>
	void SetBuff(TMessage msg);

	//
	// Private data members.
	//
private:
	//! Buffer.
	types::Buff buffer_;
};

template <typename TMessage>
void Message::SetBuff(TMessage msg)
{
	buffer_.resize(msg->ByteSize());
	if (!msg->SerializeToArray(buffer_.data(), msg->ByteSize()))
		BOOST_THROW_EXCEPTION(std::runtime_error("Failed to serialie with protobuf"));
}

}