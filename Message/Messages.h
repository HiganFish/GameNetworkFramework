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

/*
根据消息类型 将通用消息转换为对应的消息
转换完成后，自动对消息体进行解析
*/
BaseMessagePtr TransmitMessage(BaseMsgWithBufferPtr msg_ptr);

template <typename T>
std::shared_ptr<T> CastBaseMsgTo(const BaseMessagePtr& msg)
{
	return std::dynamic_pointer_cast<T>(msg);
}