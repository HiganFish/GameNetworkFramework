#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <asio.hpp>

#include "Buffer.h"
#include "BaseMessage.h"

class TcpConnection;
using OnNewMessageCallback = std::function<void(const BaseMessagePtr&)>;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

class TcpConnection : std::enable_shared_from_this<TcpConnection>
{
public:
	TcpConnection(asio::ip::tcp::socket socket);

	TcpConnection(const TcpConnection&) = delete;
	TcpConnection& operator=(const TcpConnection&) = delete;
	TcpConnection(TcpConnection&& other) noexcept:
		socket_(std::move(other.socket_))
	{
	}

	void AsyncWaitData();

	void SetOnNewMessageCallback(const OnNewMessageCallback& callback)
	{
		on_new_message_ = callback;
	}

	void SetConnectionName(const std::string& connection_name)
	{
		connection_name_ = connection_name;
	}

	const std::string& GetConnectionName() const
	{
		return connection_name_;
	}

private:
	Buffer buffer_;
	asio::ip::tcp::socket socket_;
	std::string connection_name_;

	OnNewMessageCallback on_new_message_;

	/*
	false 0 出错
	true 0 数据不足
	true n 解析长度n的数据
	*/
	std::pair<bool, size_t> DecodeBufferData();
};