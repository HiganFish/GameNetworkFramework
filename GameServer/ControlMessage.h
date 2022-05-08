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


	ControlMessage(BaseMessage&& base_message):
		BaseMessage(std::move(base_message))
	{
		DecodeBody();
	}

	ControlMessage():
		player_id(0x12345678),
		tick(0x12345678),
		control_type(ControlType::DEFAULT)
	{
		version = 1;
		message_type = MessageType::CONTROL;
	}

	uint32_t player_id;
	uint32_t tick;
	ControlType control_type;

	std::string DebugMessage() const override
	{
		return std::format("{} - player_id: {}, tick: {}, control_type: {}",
			BaseMessage::DebugMessage(), 
			player_id,
			tick,
			ControlTypeToString(control_type));
	}

private:

	virtual void EncodeData(Buffer& buffer) override
	{
		APPEND_NUMBER(buffer, player_id);
		APPEND_NUMBER(buffer, tick);
		APPEND_ENUM(buffer, control_type);
	}

	virtual void DecodeBody() override
	{
		READ_NUMBER(body_buffer, player_id);
		READ_NUMBER(body_buffer, tick);
		READ_ENUM(body_buffer, control_type);
	}
};

