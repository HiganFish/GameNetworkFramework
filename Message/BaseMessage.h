#pragma once

#include <string>
#include <memory>
#include <format>
#include <unordered_map>
#include "MessageUtils.h"

enum class MessageType : uint8_t
{
	TYTE_MIN = 0,
	DEFAULT,
	CONTROL,
	PLAYER_INIT,
	ENTER_ROOM,
	TYPE_MAX
};

static std::string MessageTypeToString(MessageType type)
{
	static std::unordered_map<MessageType, std::string> enumMap
	{ {MessageType::DEFAULT, "DEFAULT"},
	{MessageType::PLAYER_INIT, "PLAYER_INIT"},
	{MessageType::ENTER_ROOM, "ENTER_ROOM"},
	{MessageType::CONTROL, "CONTROL"}};

	auto iter = enumMap.find(type);
	return iter == enumMap.end() ? "UNKNOWN" : iter->second;
}

struct BaseMessage
{
public:

	uint32_t magic;
	uint8_t version;
	MessageType message_type;

	BaseMessage();
	virtual ~BaseMessage() = default;

	BaseMessage(BaseMessage&& old_message) noexcept;

	void EncodeMessage(Buffer& buffer);

	/*
	解析数据包
	false n 解析出错
	true 0 数据不足
	true n 解析成功 消耗n字节数据
	*/
	std::pair<bool, uint32_t> DecodeMessageHeader(Buffer& buffer,
		uint32_t* body_size = nullptr);
	virtual void DecodeMessageBody(Buffer& buffer) { }

	virtual std::string DebugMessage(const std::string& body = "") const;

private:

	// const static int HEADER_SIZE = sizeof(data_size_);
	const static int MAX_PACK_SIZE = 4 * 1024;;
	// 不含pack_size的头部长度
	const static int HEADER_SIZE_NO_LENGTH = sizeof(magic) +
		sizeof(version) + sizeof(message_type);
	const static int MAGIC = 0x11000011;

	// 数据包长度 不含pack_size本身
	// 位于序列化后数据的起始位置
	// uint32_t pack_size;
	virtual void EncodeData(Buffer& buffer) {}
};
using BaseMessagePtr = std::shared_ptr<BaseMessage>;


struct BaseMsgWithRoleId
{
	ROLE_ID role_id;
	BaseMessagePtr base_message_ptr;
};
using BaseMsgWithRoleIdPtr = std::shared_ptr<BaseMsgWithRoleId>;

struct BaseMsgWithBuffer
{
	TinyBuffer body_buffer;
	BaseMessagePtr base_message_ptr;
};
using BaseMsgWithBufferPtr = std::shared_ptr<BaseMsgWithBuffer>;

struct BaseMsgWithBufferAndId
{
	ROLE_ID role_id;
	TinyBuffer body_buffer;
	BaseMessagePtr base_message_ptr;
};
using BaseMsgWithBufferAndIdPtr = std::shared_ptr<BaseMsgWithBufferAndId>;


#define DECODE_CONSTRUCTOR(class_name) class_name \
(BaseMessage&& base_message): BaseMessage(std::move(base_message)) {}

#define ENCODE_DATA_FUNC virtual void EncodeData(Buffer& buffer) override
#define DECODE_BODY_FUNC virtual void DecodeMessageBody(Buffer& buffer) override
#define DEBUG_MSG_FUNC std::string DebugMessage(const std::string& body = "") const override
