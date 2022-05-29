#pragma once

#include "BaseMessage.h"

struct GameStartMessage : public BaseMessage
{
public:

	uint64_t timestamp;
	GameStartMessage() :
		timestamp(0x0)
	{
		version = 1;
		message_type = MessageType::GAME_START;
	}

	DECODE_CONSTRUCTOR(GameStartMessage)

	DEBUG_MSG_FUNC
	{
		return BaseMessage::DebugMessage(
			string_format("timestamp: %zu", timestamp));
	}

	DECODE_BODY_FUNC
	{
		READ_NUMBER(buffer, timestamp);
	}

private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, timestamp);
	}
};
using GameStartMessagePtr = std::shared_ptr<GameStartMessage>;