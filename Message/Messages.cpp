#pragma once
#include "Messages.h"

#define REGIST_MSG(a, b) case a:	\
{	\
	msg_ptr =	\
		SpawnNewMessage<b>(std::move(*msg_with_buffer->base_message_ptr));	\
	break;	\
}

BaseMessagePtr TransmitMessage(BaseMsgWithBufferPtr msg_with_buffer)
{
	BaseMessagePtr msg_ptr;
	switch (msg_with_buffer->base_message_ptr->message_type)
	{
		REGIST_MSG(MessageType::CONTROL, ControlMessage)
		REGIST_MSG(MessageType::PING, PingMessage)
		REGIST_MSG(MessageType::ENTER_ROOM, EnterRoomMessage)
	}
	return msg_ptr;
}