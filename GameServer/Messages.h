#pragma once

#include "BaseMessage.h"
#include "ControlMessage.h"
#include "PlayerInitMessage.h"
#include "EnterRoomMessage.h"

template <typename T, typename...Args>
BaseMessagePtr SpawnNewMessage(Args&&...args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}

BaseMsgWithRoleIdPtr TransmitMessage(BaseMsgWithBufferAndIdPtr msg_ptr);