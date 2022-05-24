#pragma once

#include "BaseMessage.h"

struct ControlMessage : public BaseMessage
{
	enum class ControlType : uint8_t
	{
		DEFAULT = 0,
		UP = 1,
		DOWN = 2,
		LEFT = 3,
		RIGHT = 4
	};
	static std::string ControlTypeToString(ControlType type)
	{
		static std::unordered_map<ControlType, std::string> enumMap
		{ {ControlType::DEFAULT, "DEFAULT"},
		 {ControlType::UP, "UP"},
		 {ControlType::DOWN, "DOWN"},
		 {ControlType::LEFT, "LEFT"},
		 {ControlType::RIGHT, "RIGHT"}};

		auto iter = enumMap.find(type);
		return iter == enumMap.end() ? "UNKNOWN" : iter->second;
	}

	uint32_t tick;
	ControlType control_type;

	ControlMessage():
		tick(0x12345678),
		control_type(ControlType::DEFAULT)
	{
		version = 1;
		message_type = MessageType::CONTROL;
	}

	DECODE_CONSTRUCTOR(ControlMessage)

	DEBUG_MSG_FUNC
	{
		return BaseMessage::DebugMessage(
			fmt::format("tick : {}, control_type : {}",
				tick,
				ControlTypeToString(control_type)));
	}

	DECODE_BODY_FUNC
	{
		READ_NUMBER(buffer, tick);
		READ_ENUM(buffer, control_type);
	}
private:

	ENCODE_DATA_FUNC
	{
		APPEND_NUMBER(buffer, tick);
		APPEND_ENUM(buffer, control_type);
	}
};
using ControlMessagePtr = std::shared_ptr<ControlMessage>;