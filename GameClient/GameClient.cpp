//
// Created by rjd
//
#include <fmt/format.h>
#include "GameClient.h"

GameClient::GameClient(const std::string& client_name):
	context_(),
	client_guard_(asio::make_work_guard(context_)),
	client_name_(client_name),
	conn_map_()
{
	run_thread_ = std::thread([this](){context_.run();});
}

bool GameClient::Connect(const std::string& conn_name, const std::string& address,
		const std::string& port,
		const OnNewMsgWithBufferFunc& func)
{
	auto iter = conn_map_.find(conn_name);
	if (iter != conn_map_.end())
	{
		return false;
	}
	auto tcp_conn = Connect(address, port, func);
	if (tcp_conn)
	{
		tcp_conn->SetConnectionName(conn_name);
		auto connection = std::make_shared<GameConnection>(tcp_conn);
		connection->SetOnNewMsgWithBufferFunc(func);
		connection->StartRecvData();
		conn_map_[conn_name] = connection;
		return true;
	}
	return false;
}

TcpConnectionPtr GameClient::Connect(const std::string& address, const std::string& port,
		const OnNewMsgWithBufferFunc& func)
{
	asio::ip::tcp::resolver resolver(context_);
	auto endpoint = resolver.resolve(address, port);
	auto sock = asio::ip::tcp::socket(context_);

	asio::error_code ec;
	asio::connect(sock, endpoint, ec);
	if (ec)
	{
		std::cout << fmt::format("connect to {}:{} failed, {}",
				address, port, ec.message()) << std::endl;
		return nullptr;
	}
	else
	{
		auto conn = std::make_shared<TcpConnection>(std::move(sock));
		return conn;
	}
}

void GameClient::AsyncSendData(const std::string& conn_name, const char* data, size_t length)
{
	auto conn = conn_map_.find(conn_name);
	if (conn != conn_map_.end())
	{
		conn->second->AsyncSendData(data, length);
	}
	else
	{
		std::cout << fmt::format("conn: {} not exist", conn_name) << std::endl;
	}
}

void GameClient::Stop()
{
	client_guard_.reset();
	context_.stop();
	run_thread_.join();
}
