#pragma once

#include "BaseMessage.h"
#include "ControlMessage.h"
#include "PlayerInitMessage.h"
#include "EnterRoomMessage.h"

BaseMsgWithRoleIdPtr TransmitMessage(BaseMsgWithBufferAndIdPtr msg_ptr);