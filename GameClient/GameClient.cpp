//
// Created by rjd
//
#include <fmt/format.h>
#include "GameClient.h"
#include "Message/Messages.h"

GameClient::GameClient(const std::string& client_name):
	context_(),
	client_guard_(asio::make_work_guard(context_)),
	client_name_(client_name),
	conn_map_(),
	recv_msg_dispatcher_(1)
{
	run_thread_ = std::thread([this](){context_.run();});
	recv_msg_dispatcher_.Start();
}

bool GameClient::Connect(const std::string& conn_name, const std::string& address, const std::string& port)
{
	auto iter = conn_map_.find(conn_name);
	if (iter != conn_map_.end())
	{
		return false;
	}
	auto tcp_conn = Connect(address, port);
	if (tcp_conn)
	{
		tcp_conn->SetConnectionName(conn_name);
		auto connection = std::make_shared<GameConnection>(tcp_conn);
		connection->SetOnNewMsgWithBufferFunc([this](auto&& ptr)
		{
			// new_pack_queue_.Push(std::move(ptr));
			auto msg = TransmitMessage(ptr);
			recv_msg_dispatcher_.Push(msg->role_id, msg);
		});
		connection->StartRecvData();
		conn_map_[conn_name] = connection;
		return true;
	}
	return false;
}

TcpConnectionPtr GameClient::Connect(const std::string& address, const std::string& port)
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

void GameClient::Stop()
{
	recv_msg_dispatcher_.Stop();
	client_guard_.reset();
	context_.stop();
	run_thread_.join();
}

void GameClient::SetMsgCallback(MessageType type, const MsgDispatcher::MsgCallback& callback)
{
	recv_msg_dispatcher_.SetMsgCallback(type, callback);
}

void GameClient::SendMsg(const std::string& conn_name, const BaseMessagePtr& msg_ptr)
{
	auto conn = conn_map_.find(conn_name);
	if (conn != conn_map_.end())
	{
		TinyBuffer buffer;
		msg_ptr->EncodeMessage(buffer);
		conn->second->AsyncSendData(buffer.ReadBegin(), buffer.ReadableSize());
	}
	else
	{
		std::cout << fmt::format("conn: {} not exist", conn_name) << std::endl;
	}
}
