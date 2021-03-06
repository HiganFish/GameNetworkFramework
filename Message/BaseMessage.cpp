#include <iostream>

#include "BaseMessage.h"

BaseMessage::BaseMessage() :
	magic(MAGIC),
	message_unique_id(0),
	role_id(0),
	version(0x00),
	message_type(MessageType::DEFAULT)
{
}

BaseMessage::BaseMessage(BaseMessage&& old_message) noexcept :
	magic(old_message.magic),
	message_unique_id(old_message.message_unique_id),
	role_id(old_message.role_id),
	version(old_message.version),
	message_type(old_message.message_type)
{
}

void BaseMessage::EncodeMessage(Buffer& buffer)
{
	size_t old_size = buffer.ReadableSize();
	APPEND_NUMBER(buffer, magic);
	APPEND_NUMBER(buffer, message_unique_id);
	APPEND_NUMBER(buffer, role_id);
	APPEND_NUMBER(buffer, version);
	APPEND_ENUM(buffer, message_type);

	EncodeData(buffer);

	// put pack length at begin pos
	buffer.PreAppendNumber(
		static_cast<uint32_t>(buffer.ReadableSize() - old_size));
}

std::pair<bool, uint32_t> BaseMessage::DecodeMessageHeader(Buffer& buffer, uint32_t* body_size)
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
		std::cout << "pack_size > MAX_PACK_SIZE" << std::endl;
		return { false, 0 };
	}
	if (buffer.ReadableSize() < pack_size + sizeof(pack_size))
	{
		return { true, 0 };
	}
	buffer.AddReadIndex(sizeof(pack_size));


	PEEK_NUMBER(buffer, magic);
	if (magic != MAGIC)
	{
		std::cout << "magic != MAGIC" << std::endl;
		return { false, 0 };
	}
	buffer.AddReadIndex(sizeof(magic));

	READ_NUMBER(buffer, message_unique_id);
	READ_NUMBER(buffer, role_id);
	READ_NUMBER(buffer, version);
	READ_ENUM(buffer, message_type);

	if (message_type <= MessageType::TYTE_MIN ||
		message_type >= MessageType::TYPE_MAX)
	{
		return { false, 0 };
	}
	if (body_size)
	{
		*body_size = pack_size - HEADER_SIZE_NO_LENGTH;
	}

	return { true, static_cast<uint32_t>(
		old_size - buffer.ReadableSize()) };
}

std::string BaseMessage::DebugMessage(const std::string& body) const
{
	if (body.empty())
	{
		return string_format("[ version: %d, role_id: %d, message_type: %s ]",
				version, role_id, MessageTypeToString(message_type).c_str());
	}
	return string_format("[ version: %d, role_id: %d, message_type: %s ] - %s",
		version, role_id, MessageTypeToString(message_type).c_str(), body.c_str());
}