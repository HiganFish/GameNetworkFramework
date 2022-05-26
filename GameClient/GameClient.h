//
// Created by rjd
//

#ifndef GAMEPROJECT_GAMECLIENT_H
#define GAMEPROJECT_GAMECLIENT_H

#include <string>
#include <unordered_map>
#include "Network/GameConnection.h"
#include "Network/NetworkCommon.h"
#include "Network/MsgDispatcher.h"

class GameClient
{
public:
	explicit GameClient(const std::string& client_name);
	~GameClient() = default;

	GameClient(const GameClient&) = delete;
	GameClient& operator=(const GameClient&) = delete;

	bool Connect(const std::string& conn_name, const std::string& address, const std::string& port);


	void Stop();

	void SetMsgCallback(MessageType type, const MsgDispatcher::MsgCallback& callback);

	void SendMsg(const std::string& conn_name, const BaseMessagePtr& msg_ptr);

private:
	asio::io_context context_;
	asio::executor_work_guard<asio::io_context::executor_type> client_guard_;

	std::thread run_thread_;

	std::string client_name_;

	MsgDispatcher recv_msg_dispatcher_;

	std::unordered_map<std::string, GameConnectionPtr> conn_map_;

	TcpConnectionPtr Connect(const std::string& address, const std::string& port);
};
using GameClientPtr = std::shared_ptr<GameClient>;

#endif //GAMEPROJECT_GAMECLIENT_H
