#include <iostream>

#include "BaseMessage.h"

BaseMessage::BaseMessage() :
	magic(666666666),
	version(0x00),
	message_type(MessageType::DEFAULT)
{
}

BaseMessage::BaseMessage(BaseMessage&& old_message) noexcept :
	magic(old_message.magic),
	version(old_message.version),
	message_type(old_message.message_type),
	body_buffer(std::move(old_message.body_buffer))
{
}

void BaseMessage::EncodeMessage(Buffer& buffer)
{
	size_t old_size = buffer.ReadableSize();
	APPEND_NUMBER(buffer, magic);
	APPEND_NUMBER(buffer, version);
	APPEND_ENUM(buffer, message_type);

	EncodeData(buffer);

	// 数据包长度放于起始位置
	buffer.PreAppendNumber(
		static_cast<uint32_t>(buffer.ReadableSize() - old_size));
}

std::pair<bool, uint32_t> BaseMessage::DecodeMessage(Buffer& buffer)
{
	uint32_t old_size = buffer.ReadableSize();
	uint32_t pack_size = 0;
	if (old_size < sizeof(pack_size))
	{
		return { true, 0 };
	}

	PEEK_NUMBER(buffer, pack_size);
	if (pack_size > MAX_PACK_SIZE)
	{
		return { false, 0 };
	}
	// data_size 不包含其本身大小
	if (buffer.ReadableSize() < pack_size + sizeof(pack_size))
	{
		return { true, 0 };
	}
	buffer.AddReadIndex(sizeof(pack_size));


	READ_NUMBER(buffer, magic);
	READ_NUMBER(buffer, version);
	READ_ENUM(buffer, message_type);

	if (message_type <= MessageType::TYTE_MIN ||
		message_type >= MessageType::TYPE_MAX)
	{
		return { false, 0 };
	}

	uint32_t body_size = pack_size - HEADER_SIZE_NO_LENGTH;
	body_buffer.AppendData(buffer.ReadBegin(), body_size);
	buffer.AddReadIndex(body_size);
	DecodeBody();

	return { true, static_cast<uint32_t>(
		old_size - buffer.ReadableSize()) };
}

std::string BaseMessage::DebugMessage() const
{
	return std::format("[ magic: {}, version: {}, message_type: {} ]",
			magic, version, MessageTypeToString(message_type));
}