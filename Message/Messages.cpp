#pragma once
#include "Messages.h"

#define REGIST_MSG(a, b) case a:	\
{	\
	msg->base_message_ptr =	\
		SpawnNewMessage<b>(std::move(*msg_ptr->base_message_ptr));	\
	break;	\
}

BaseMsgWithRoleIdPtr TransmitMessage(BaseMsgWithBufferAndIdPtr msg_ptr)
{
	auto msg = std::make_shared<BaseMsgWithRoleId>();
	msg->role_id = msg_ptr->role_id;

	switch (msg_ptr->base_message_ptr->message_type)
	{
		REGIST_MSG(MessageType::CONTROL, ControlMessage)
		REGIST_MSG(MessageType::PLAYER_INIT, PlayerInitMessage)
		REGIST_MSG(MessageType::ENTER_ROOM, PlayerInitMessage)
	}
	return msg;
}