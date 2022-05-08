#pragma once

#include "Buffer.h"
#include "NetworkCommon.h"

class TinyBuffer : public Buffer
{
public:
	TinyBuffer() :
		Buffer(TINY_BUFFER_DEFAULT_BUFFER_SIZE)
	{
	}
};

#define APPEND_NUMBER(buffer, number) buffer.AppendNumber<decltype(number)>(number)
#define APPEND_ENUM(buffer, e) APPEND_NUMBER(buffer, std::to_underlying(e))
#define READ_NUMBER(buffer, var) var = buffer.ReadNumber<decltype(var)>()
#define PEEK_NUMBER(buffer, var) var = buffer.PeekNumber<decltype(var)>()
#define READ_ENUM(buffer, var) var = static_cast<decltype(var)>(buffer.ReadNumber<decltype(std::to_underlying(var))>())