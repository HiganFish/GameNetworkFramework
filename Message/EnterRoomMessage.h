#pragma once

#include "BaseMessage.h"

class EnterRoomMessage : public BaseMessage
{
public:
	uint32_t room_id;
	int32_t result;

	EnterRoomMessage() :
		room_id(0),
		result(0)
	{
		message_type = MessageType::ENTER_ROOM;
	}

	DECODE_CONSTRUCTOR(EnterRoomMessage)

		DEBUG_MSG_FUNC
	{
		return BaseMessage::DebugMessage(
			fmt::format("room_id: {}", room_id));
	}

	DECODE_BODY_FUNC
	{
		READ_NUMBER(buffer, room_id);
		READ_NUMBER(buffer, result);
	}
private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, room_id);
		APPEND_NUMBER(buffer, result);
	}

};
using EnterRoomMessagePtr = std::shared_ptr<EnterRoomMessage>;