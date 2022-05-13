#pragma once

#include "BaseMessage.h"

struct PingMessage : public BaseMessage
{
public:

	uint64_t timestamp;
	PingMessage() :
		timestamp(0x0)
	{
		version = 1;
		message_type = MessageType::PING;
	}

	DECODE_CONSTRUCTOR(PingMessage)

	DEBUG_MSG_FUNC
	{
		return BaseMessage::DebugMessage(
			std::format("role_id: {}", role_id));
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
using PlayerInitMessagePtr = std::shared_ptr<PingMessage>;