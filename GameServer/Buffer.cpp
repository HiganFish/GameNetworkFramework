#include <cstring>

#include "Buffer.h"

Buffer::Buffer(size_t buffer_size) :
	buffer_(buffer_size + IDX_BEGIN),
	read_idx_(IDX_BEGIN),
	write_idx_(IDX_BEGIN)
{
}

Buffer::Buffer(Buffer&& old_buffer) noexcept:
	write_idx_(old_buffer.write_idx_),
	read_idx_(old_buffer.read_idx_),
	buffer_(std::move(old_buffer.buffer_))
{
}


Buffer::~Buffer()
{
}

size_t Buffer::ReadableSize() const
{
	return write_idx_ - read_idx_;
}

size_t Buffer::WritableSize() const
{
	return buffer_.size() - write_idx_;
}

void Buffer::AddWriteIndex(size_t index)
{
	write_idx_ += index;
}

void Buffer::AddReadIndex(size_t index)
{
	if (index > ReadableSize())
	{
		read_idx_ += ReadableSize();
	}
	else
	{
		read_idx_ += index;
	}

}

char* Buffer::WriteBegin()
{
	return &buffer_[write_idx_];
}

const char* Buffer::ReadBegin() const
{
	return &buffer_[read_idx_];
}

void Buffer::AdjustBuffer()
{
	size_t readable_length = ReadableSize();

	std::copy(&buffer_[read_idx_], &buffer_[write_idx_], &buffer_[IDX_BEGIN]);

	read_idx_ = IDX_BEGIN;
	write_idx_ = IDX_BEGIN + readable_length;
}

void Buffer::Reset()
{
	read_idx_ = IDX_BEGIN;
	write_idx_ = IDX_BEGIN;
}

std::string Buffer::ReadAllAsString()
{
	std::string result(ReadBegin(), ReadableSize());
	AddReadIndex(ReadableSize());
	return result;
}

void Buffer::ReSize(size_t new_size)
{
	buffer_.resize(new_size + IDX_BEGIN);
}

void Buffer::SwapBuffer(Buffer* buffer)
{
	if (!buffer)
	{
		return;
	}

	buffer_.swap(buffer->buffer_);
	Swap(read_idx_, buffer->read_idx_);
	Swap(write_idx_, buffer->write_idx_);
}

void Buffer::Swap(size_t& lhs, size_t& rhs)
{
	size_t temp = rhs;
	rhs = lhs;
	lhs = temp;
}

void Buffer::DropAllData()
{
	AddReadIndex(ReadableSize());
}

void Buffer::AppendData(const void* data, size_t length)
{
	AdjustBuffer();
	if (length > WritableSize())
	{
		size_t remain = length - WritableSize();
		ReSize(buffer_.size() + remain);
	}

	memcpy(WriteBegin(), data, length);
	AddWriteIndex(length);
}

void Buffer::AppendData(const std::string_view& data)
{
	AppendData(data.data(), data.size());
}