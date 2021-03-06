#pragma once

#include <vector>
#include <string>
#include <cassert>
#include <utility>

#include "ByteOrder.h"

constexpr int DEFAULT_BUFFER_SIZE = 4096;
constexpr int IDX_BEGIN = 8;

/**
 * common buffer
*/
class Buffer
{
public:
	explicit Buffer(size_t buffer_size = DEFAULT_BUFFER_SIZE);
	Buffer(Buffer&& old_buffer) noexcept;
	~Buffer();

	size_t ReadableSize() const;
	size_t WritableSize() const;

	void AddWriteIndex(size_t index);
	void AddReadIndex(size_t index);

	void DropAllData();

	char* WriteBegin();

	const char* ReadBegin() const;

	void AdjustBuffer();

	void Reset();

	void ReSize(size_t new_size);

	void AppendData(const void* data, size_t length);

	void AppendData(const std::string_view& data);

	template <typename T>
	void AppendNumber(T number)
	{
		T n = hton(number);
		AppendData(&n, sizeof n);
	}

	template <typename T>
	void PreAppendNumber(T number)
	{
		T n = hton(number);
		PreAppendData(&n, sizeof n);
	}

	void PreAppendData(const void* data, size_t length)
	{
		assert(read_idx_ >= length);
		read_idx_ -= length;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + length, Begin() + read_idx_);
	}

	template <typename T>
	T ReadNumber()
	{
		T result = PeekNumber<T>();
		AddReadIndex(sizeof(T));
;		return result;
	}

	template <typename T>
	T PeekNumber()
	{
		assert(ReadableSize() >= sizeof(T));
		T result;
		memcpy(&result, ReadBegin(), sizeof(T));
		return ntoh(result);
	}

	std::string ReadAllAsString();

	void SwapBuffer(Buffer* buffer);

protected:

	std::vector<char> buffer_;
	size_t read_idx_;
	size_t write_idx_;

private:
	void Swap(size_t& lhs, size_t& rhs);

	char* Begin()
	{
		return &*buffer_.begin();
	}

	const char* Begin() const
	{
		return &*buffer_.begin();
	}
};

#define APPEND_NUMBER(buffer, number) buffer.AppendNumber<decltype(number)>(number)
#define READ_NUMBER(buffer, var) var = buffer.ReadNumber<decltype(var)>()
#define PEEK_NUMBER(buffer, var) var = buffer.PeekNumber<decltype(var)>()

//#define ENUM_TO_NUM(e) std::to_underlying(e)
//#define APPEND_ENUM(buffer, e) APPEND_NUMBER(buffer, ENUM_TO_NUM(e))
//#define READ_ENUM(buffer, var) var = static_cast<decltype(var)>(buffer.ReadNumber<decltype(std::to_underlying(var))>())

#define ENUM_TO_NUM(e) static_cast<int>(e)
#define APPEND_ENUM(buffer, e) APPEND_NUMBER(buffer, ENUM_TO_NUM(e))
#define READ_ENUM(buffer, var) var = static_cast<decltype(var)>((buffer).ReadNumber<int>())
