#pragma once

#include "BaseMessage.h"

struct ControlMessage : public BaseMessage
{
	enum class MoveDirection : uint8_t
	{
		NONE = 0,
		UP = 1,
		DOWN = 2,
		LEFT = 3,
		RIGHT = 4
	};
	static std::string ControlTypeToString(MoveDirection type)
	{
		static std::unordered_map<MoveDirection, std::string> enumMap
		{ { MoveDirection::NONE,  "NONE"},
		 {  MoveDirection::UP,    "UP"},
		 {  MoveDirection::DOWN,  "DOWN"},
		 {  MoveDirection::LEFT,  "LEFT"},
		 {  MoveDirection::RIGHT, "RIGHT"}};

		auto iter = enumMap.find(type);
		return iter == enumMap.end() ? "UNKNOWN" : iter->second;
	}

	uint32_t tick;
	MoveDirection move_direction;

	ControlMessage():
			tick(0x12345678),
			move_direction(MoveDirection::NONE)
	{
		version = 1;
		message_type = MessageType::CONTROL;
	}

	DECODE_CONSTRUCTOR(ControlMessage)

	DEBUG_MSG_FUNC
	{
		return BaseMessage::DebugMessage(
			string_format("tick : %d, move_direction : %s",
				tick,
				ControlTypeToString(move_direction).c_str()));
	}

	DECODE_BODY_FUNC
	{
		READ_NUMBER(buffer, tick);
		READ_ENUM(buffer, move_direction);
	}
private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, tick);
		APPEND_ENUM(buffer, move_direction);
	}
};
using ControlMessagePtr = std::shared_ptr<ControlMessage>;