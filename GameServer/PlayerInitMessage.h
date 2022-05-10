#pragma once

#include "BaseMessage.h"

struct PlayerInitMessage : public BaseMessage
{
public:

	uint32_t player_id;
	PlayerInitMessage() :
		player_id(0x12345678)
	{
		version = 1;
		message_type = MessageType::PLAYER_INIT;
	}

	DECODE_CONSTRUCTOR(PlayerInitMessage)

	DEBUG_MSG_FUNC
	{
	}

private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, player_id);
	}

	DECODE_DATA_FUNC
	{
		READ_NUMBER(body_buffer, player_id);
	}
};