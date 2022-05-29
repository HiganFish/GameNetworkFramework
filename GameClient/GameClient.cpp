//
// Created by rjd
//
#include <thread>
#include <condition_variable>
#include "GameClient.h"
#include "Message/Messages.h"
#include "Utils/TimeUtils.h"

GameClient::GameClient(const std::string& client_name):
		context_(),
		client_guard_(asio::make_work_guard(context_)),
		client_name_(client_name),
		conn_map_(),
		recv_msg_dispatcher_(1, true),
		delay_ms_(0),
		ping_message_ptr_(nullptr),
		running_(true),
		has_delay_(false)
{
	run_thread_ = std::thread([this](){context_.run();});

	recv_msg_dispatcher_.SetMsgCallback(MessageType::PING,
			[this](ROLE_ID role_id, const BaseMessagePtr& ping_msg_ptr)
			{
				PingMessagePtr ptr = CastBaseMsgTo<PingMessage>(ping_msg_ptr);
				delay_ms_ = NOW_MS - ptr->timestamp;
				if (!has_delay_)
				{
					std::unique_lock lock(delay_variable_mutex_);
					has_delay_ = true;
					delay_variable_.notify_all();
				}

			});
	recv_msg_dispatcher_.SetMsgCallback(MessageType::GAME_START,
			[this](ROLE_ID role_id, const BaseMessagePtr& start_msg)
			{
				GameStartMessagePtr ptr = CastBaseMsgTo<GameStartMessage>(start_msg);
				game_start_promise_.set_value(ptr->timestamp);
			});

	recv_msg_dispatcher_.Start();
}

GameClient::~GameClient()
{
	if (running_)
	{
		Stop();
	}
}

bool GameClient::Connect(ROLE_ID role_id, const std::string& address, const std::string& port)
{
	auto iter = conn_map_.find(role_id);
	if (iter != conn_map_.end())
	{
		return false;
	}
	auto tcp_conn = Connect(address, port);
	if (tcp_conn)
	{
		tcp_conn->SetConnectionName(client_name_ + "-"+ std::to_string(role_id));
		auto connection = std::make_shared<GameConnection>(tcp_conn);
		connection->SetOnNewMsgWithBufferFunc([this](auto&& ptr)
		{
			// new_pack_queue_.Push(std::move(ptr));
			auto msg = TransmitMessage(ptr);
			recv_msg_dispatcher_.Push(msg->role_id, msg);
		});
		connection->StartRecvData();
		conn_map_[role_id] = connection;
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
		LOG_INFO << string_format("connect to %s:%s failed, %s",
				address.c_str(), port.c_str(), ec.message().c_str());
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
	running_ = false;
	recv_msg_dispatcher_.Stop();
	client_guard_.reset();
	context_.stop();
	run_thread_.join();
}

void GameClient::SetMsgCallback(MessageType type, const MsgDispatcher::MsgCallback& callback)
{
	recv_msg_dispatcher_.SetMsgCallback(type, callback);
}

void GameClient::SendMsgAsync(ROLE_ID role_id, const BaseMessagePtr& msg_ptr)
{
	auto conn = conn_map_.find(role_id);
	if (conn != conn_map_.end())
	{
		TinyBuffer buffer;
		msg_ptr->EncodeMessage(buffer);
		conn->second->AsyncSendData(buffer.ReadBegin(), buffer.ReadableSize());
	}
	else
	{
		LOG_INFO << "conn: "<< role_id << " not exist";
	}
}

void GameClient::TestDelay(int32_t role_id)
{
	if (!ping_message_ptr_)
	{
		ping_message_ptr_ = std::make_shared<PingMessage>();
	}
	ping_message_ptr_->role_id = role_id;
	ping_message_ptr_->timestamp = NOW_MS;
	SendMsgAsync(role_id, ping_message_ptr_);
}

uint32_t GameClient::GetDelayMs()
{
	if (!has_delay_)
	{
		std::unique_lock lock(delay_variable_mutex_);
		delay_variable_.wait(lock, [this](){return has_delay_;});
	}
	return delay_ms_;
}

uint64_t GameClient::WaitForGameStart(int32_t role_id)
{
	auto f = game_start_promise_.get_future();
	return f.get();
}

BaseMessagePtr GameClient::SendMsgSync(int32_t role_id, const BaseMessagePtr& msg_ptr)
{
	auto future = recv_msg_dispatcher_.SetSyncMessageId(role_id, msg_ptr);
	SendMsgAsync(role_id, msg_ptr);
	return future.get();
}

bool GameClient::EnterRoom(ROLE_ID role_id, uint32_t room_id)
{
	EnterRoomMessagePtr msg = SpawnNewMessage<EnterRoomMessage>();
	msg->role_id = role_id;
	msg->room_id = room_id;
	auto base_rsp = SendMsgSync(role_id, msg);
	EnterRoomMessagePtr rsp = CastBaseMsgTo<EnterRoomMessage>(base_rsp);
	return rsp->result == 0;
}
