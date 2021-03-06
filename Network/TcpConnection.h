#pragma once

#include <vector>
#include <memory>
#include <iostream>
#include <functional>

#include "Buffer/Buffer.h"
#include "Message/BaseMessage.h"
#include "NetworkCommon.h"

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
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

	void AsyncSendData(const char* data, size_t length);

	void SetOnNewDataFunc(const OnNewDataFunc& func);

	void SetOnConnCloseFunc(const OnTcpConnectionCloseFunc& onConnCloseFunc);

	void SetConnectionName(const std::string& connection_name);

	const std::string& GetConnectionName() const;

	asio::ip::tcp::socket& GetSocket()
	{
		return socket_;
	}

private:
	Buffer buffer_;
	asio::ip::tcp::socket socket_;
	std::string connection_name_;

	OnNewDataFunc on_new_data_func_;

	OnTcpConnectionCloseFunc on_conn_close_func_;
};