#pragma once

#include "Buffer/Buffer.h"

constexpr int TINY_BUFFER_DEFAULT_BUFFER_SIZE = 64;

class TinyBuffer : public Buffer
{
public:
	TinyBuffer() :
		Buffer(TINY_BUFFER_DEFAULT_BUFFER_SIZE)
	{
	}
};

#define ROLE_ID int32_t