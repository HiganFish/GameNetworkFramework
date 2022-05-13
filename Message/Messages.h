#pragma once

#include "BaseMessage.h"
#include "ControlMessage.h"
#include "PingMessage.h"
#include "EnterRoomMessage.h"

template <typename T, typename...Args>
BaseMessagePtr SpawnNewMessage(Args&&...args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

BaseMessagePtr TransmitMessage(BaseMsgWithBufferPtr msg_ptr);