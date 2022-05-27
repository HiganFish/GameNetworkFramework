#pragma once

#include "TcpConnection.h"

class GameConnection : public std::enable_shared_from_this<GameConnection>
{
public:
	GameConnection(const TcpConnectionPtr& connection);
	GameConnection(const GameConnection&) = delete;
	GameConnection& operator=(const GameConnection&) = delete;

	const std::string& GetConnectionName();

	void SetOnNewMsgWithBufferFunc(const OnNewMsgWithBufferFunc& func);

	void AsyncSendData(const char* data, size_t length);

	void StartRecvData();

	void SetRegisterFunc(const RegisterRoleIdConnFunc& func);

	void SetOnGameConnCloseFunc(const OnGameConnectionCloseFunc& onGameConnCloseFunc);

	int32_t GetRoleId() const;

private:
	TcpConnectionPtr tcp_connection_;
	std::string connection_name_;
	ROLE_ID role_id_;
	RegisterRoleIdConnFunc register_func_;
	OnNewMsgWithBufferFunc on_new_msg_with_buf_func_;

	OnGameConnectionCloseFunc on_game_conn_close_func_;

	bool recving_data_;

	void OnNewData(const TcpConnectionPtr& connection, Buffer& buffer);
};
