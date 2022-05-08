#pragma once

#include "TcpConnection.h"

class GameConnection;
using GameConnectionPtr = std::shared_ptr<GameConnection>;
class GameConnection : public std::enable_shared_from_this<GameConnection>
{
public:
	GameConnection(const TcpConnectionPtr& connection);

	const std::string& GetConnectionName()
	{
		return connection_name_;
	}

private:
	TcpConnectionPtr tcp_connection_;
	std::string connection_name_;

	void OnNewMessage(const BaseMessagePtr& message_ptr);
};
