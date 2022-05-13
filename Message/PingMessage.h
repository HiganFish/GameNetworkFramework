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
			std::format("timestamp: {}", timestamp));
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
using PingMessagePtr = std::shared_ptr<PingMessage>;