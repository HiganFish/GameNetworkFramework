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
	~GameClient();

	GameClient(const GameClient&) = delete;
	GameClient& operator=(const GameClient&) = delete;

	bool Connect(ROLE_ID role_id, const std::string& address, const std::string& port);


	void Stop();

	void SetMsgCallback(MessageType type, const MsgDispatcher::MsgCallback& callback);

	void SendMsg(ROLE_ID role_id, const BaseMessagePtr& msg_ptr);

	void TestDelay(ROLE_ID role_id);
	int32_t GetDelayMs() const;

private:
	asio::io_context context_;
	asio::executor_work_guard<asio::io_context::executor_type> client_guard_;

	std::thread run_thread_;

	std::string client_name_;

	MsgDispatcher recv_msg_dispatcher_;

	std::unordered_map<ROLE_ID, GameConnectionPtr> conn_map_;

	uint32_t delay_ms_;
	PingMessagePtr ping_message_ptr_;

	bool running_;

	TcpConnectionPtr Connect(const std::string& address, const std::string& port);
};
using GameClientPtr = std::shared_ptr<GameClient>;

#endif //GAMEPROJECT_GAMECLIENT_H
