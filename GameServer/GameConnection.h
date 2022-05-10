#pragma once

#include "TcpConnection.h"

class GameConnection : public std::enable_shared_from_this<GameConnection>
{
public:
	GameConnection(const TcpConnectionPtr& connection);
	GameConnection(const GameConnection&) = delete;
	GameConnection& operator=(const GameConnection&) = delete;

	const std::string& GetConnectionName();

	void SetOnNewMsgWithBufferAndIdFunc(const OnNewMsgWithBufferAndIdFunc& func);

	void AsyncSendData(const char* data, size_t length);

	void StartRecvData();

	void SetRegisterFunc(const RegisterRoleIdConnFunc& func);

private:
	TcpConnectionPtr tcp_connection_;
	std::string connection_name_;
	ROLE_ID role_id_;
	RegisterRoleIdConnFunc register_func_;
	OnNewMsgWithBufferAndIdFunc on_new_msg_with_buf_and_id_func_;

	bool recving_data_;

	void OnNewData(const TcpConnectionPtr& connection, Buffer& buffer);
};
