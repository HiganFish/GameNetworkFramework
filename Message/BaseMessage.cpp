#include <iostream>

#include "BaseMessage.h"

BaseMessage::BaseMessage() :
	magic(MAGIC),
	role_id(0),
	version(0x00),
	message_type(MessageType::DEFAULT)
{
}

BaseMessage::BaseMessage(BaseMessage&& old_message) noexcept :
	magic(old_message.magic),
	role_id(old_message.role_id),
	version(old_message.version),
	message_type(old_message.message_type)
{
}

void BaseMessage::EncodeMessage(Buffer& buffer)
{
	size_t old_size = buffer.ReadableSize();
	APPEND_NUMBER(buffer, magic);
	APPEND_NUMBER(buffer, role_id);
	APPEND_NUMBER(buffer, version);
	APPEND_ENUM(buffer, message_type);

	EncodeData(buffer);

	// 数据包长度放于起始位置
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
	// 数据过大
	if (pack_size > MAX_PACK_SIZE)
	{
		std::cout << "pack_size > MAX_PACK_SIZE" << std::endl;
		return { false, 0 };
	}
	// data_size 不包含其本身大小 数据不足
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
	std::string fmt = "[ version: {}, role_id: {}, message_type: {} ] - {}";
	if (body.empty())
	{
		fmt = "[ version: {}, role_id: {}, message_type: {} ]";
	}
	return std::format(fmt,
		version, role_id, MessageTypeToString(message_type), body);
}