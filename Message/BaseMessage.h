#pragma once

#include <string>
#include <memory>
#include <fmt/format.h>
#include <unordered_map>
#include "MessageUtils.h"

enum class MessageType : uint8_t
{
	TYTE_MIN = 0,
	DEFAULT,
	CONTROL,
	PING,
	ENTER_ROOM,
	GAME_START,
	TYPE_MAX
};

static std::string MessageTypeToString(MessageType type)
{
	static std::unordered_map<MessageType, std::string> enumMap
	{ {MessageType::DEFAULT, "NONE"},
	{MessageType::PING, "PING"},
	{MessageType::ENTER_ROOM, "ENTER_ROOM"},
	{MessageType::GAME_START, "GAME_START"},
	{MessageType::CONTROL, "CONTROL"}};

	auto iter = enumMap.find(type);
	return iter == enumMap.end() ? "UNKNOWN" : iter->second;
}

struct BaseMessage
{
public:

	uint32_t magic;
	uint32_t message_unique_id;
	ROLE_ID role_id;
	uint8_t version;
	MessageType message_type;

	BaseMessage();
	virtual ~BaseMessage() = default;

	BaseMessage(BaseMessage&& old_message) noexcept;

	void EncodeMessage(Buffer& buffer);

	std::pair<bool, uint32_t> DecodeMessageHeader(Buffer& buffer,
		uint32_t* body_size = nullptr);
	virtual void DecodeMessageBody(Buffer& buffer) { }

	virtual std::string DebugMessage(const std::string& body = "") const;

private:

	const static int MAX_PACK_SIZE = 4 * 1024;;
	const static int HEADER_SIZE_NO_LENGTH = sizeof(message_unique_id) +
			sizeof(magic) +
		sizeof(role_id) + 
		sizeof(version) + sizeof(ENUM_TO_NUM(message_type));
	const static int MAGIC = 0x11000011;

	virtual void EncodeData(Buffer& buffer) {}
};
using BaseMessagePtr = std::shared_ptr<BaseMessage>;

struct BaseMsgWithBuffer
{
	TinyBuffer body_buffer;
	BaseMessagePtr base_message_ptr;
};
using BaseMsgWithBufferPtr = std::shared_ptr<BaseMsgWithBuffer>;

struct BaseMsgSendPack
{
	std::vector<ROLE_ID> send_to;
	BaseMessagePtr base_message_ptr;
};
using BaseMsgSendPackPtr = std::shared_ptr<BaseMsgSendPack>;

#define DECODE_CONSTRUCTOR(class_name) class_name \
(BaseMessage&& base_message): BaseMessage(std::move(base_message)) {}

#define ENCODE_DATA_FUNC virtual void EncodeData(Buffer& buffer) override
#define DECODE_BODY_FUNC virtual void DecodeMessageBody(Buffer& buffer) override
#define DEBUG_MSG_FUNC std::string DebugMessage(const std::string& body = "") const override
