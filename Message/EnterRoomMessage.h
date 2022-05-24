#pragma once

#include "BaseMessage.h"

class EnterRoomMessage : public BaseMessage
{
public:
	int32_t room_id;

	EnterRoomMessage() :
		room_id(0)
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
	}
private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, room_id);
	}

};