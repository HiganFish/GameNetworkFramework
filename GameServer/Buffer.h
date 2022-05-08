#ifndef UTILS_BUFFER_H
#define UTILS_BUFFER_H

#include <vector>
#include <string>
#include <cassert>

#include "ByteOrder.h"

constexpr int DEFAULT_BUFFER_SIZE = 4096;
constexpr int IDX_BEGIN = 8;

/**
 * 通用缓冲区
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

	/**
	 * 丢弃所有未读数据
	 */
	void DropAllData();

	char* WriteBegin();

	const char* ReadBegin() const;

	void AdjustBuffer();

	/**
	 * 重置Buffer 清除所有数据 重置总读写字节数
	 */
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
		AddReadIndex(sizeof T);
;		return result;
	}

	template <typename T>
	T PeekNumber()
	{
		assert(ReadableSize() >= sizeof T);
		T result;
		memcpy(&result, ReadBegin(), sizeof T);
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
		return buffer_.data();
	}
};


#endif // !UTILS_BUFFER_H