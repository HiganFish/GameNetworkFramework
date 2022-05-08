#pragma once

#include <string>
#include <memory>
#include <format>
#include <unordered_map>
#include "MessageUtils.h"

enum class MessageType : uint8_t
{
	DEFAULT = 0,
	CONTROL = 20
};
static std::string MessageTypeToString(MessageType type)
{
	static std::unordered_map<MessageType, std::string> enumMap
	{ {MessageType::DEFAULT, "DEFAULT"},
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

	BaseMessage(BaseMessage&& old_message) noexcept;

	void EncodeMessage(Buffer& buffer);

	/*
	解析数据包
	false n 解析出错
	true 0 数据不足
	true n 解析成功 消耗n字节数据
	*/
	std::pair<bool, uint32_t> DecodeMessage(Buffer& buffer);

	virtual std::string DebugMessage() const;

protected:
	TinyBuffer body_buffer;

private:

	// const static int HEADER_SIZE = sizeof(data_size_);
	const static int MAX_PACK_SIZE = 4 * 1024;;
	// 不含pack_size的头部长度
	const static int HEADER_SIZE_NO_LENGTH = sizeof(magic) +
		sizeof(version) + sizeof(message_type);

	// 数据包长度 不含pack_size本身
	// 位于序列化后数据的起始位置
	// uint32_t pack_size;
	virtual void EncodeData(Buffer& buffer) {}
	virtual void DecodeBody() { }
};


using BaseMessagePtr = std::shared_ptr<BaseMessage>;
template <typename T, typename...Args>
BaseMessagePtr SpawnNewMessage(Args&&...args)
{
	return std::make_shared<T>(std::forward<Args>(args)...);
}