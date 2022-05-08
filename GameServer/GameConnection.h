#pragma once

#include "TcpConnection.h"

class GameConnection : public std::enable_shared_from_this<GameConnection>
{
public:
	GameConnection(const TcpConnectionPtr& connection);

	const std::string& GetConnectionName();

	void SetOnNewMsgWithIdFunc(const OnNewMsgWithIdFunc& func);

	void AsyncSendData(const char* data, size_t length);

	void StartRecvData();

private:
	TcpConnectionPtr tcp_connection_;
	std::string connection_name_;
	ROLE_ID role_id_;
	OnNewMsgWithIdFunc on_new_msg_with_id_func_;

	bool recving_data_;

	void OnNewData(const TcpConnectionPtr& connection, Buffer& buffer);
};
