#pragma once

#include "BaseMessage.h"

struct PlayerInitMessage : public BaseMessage
{
public:

	ROLE_ID role_id;
	PlayerInitMessage() :
		role_id(0x12345678)
	{
		version = 1;
		message_type = MessageType::PLAYER_INIT;
	}

	DECODE_CONSTRUCTOR(PlayerInitMessage)

	DEBUG_MSG_FUNC
	{
		return BaseMessage::DebugMessage(
			std::format("role_id: {}", role_id));
	}

	DECODE_BODY_FUNC
	{
		READ_NUMBER(buffer, role_id);
	}

	void DecodeMsgBody(const char* data, size_t length)
	{
		TinyBuffer buffer;
		buffer.AppendData(data, length);
		READ_NUMBER(buffer, role_id);
	}

private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, role_id);
	}
};