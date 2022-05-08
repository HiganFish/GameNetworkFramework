#pragma once

#include "AsioServer.h"
#include "GameConnection.h"

class GameServer : public AsioServer
{
public:

    GameServer(const std::string& server_name, short port);

    virtual void OnNewTcpConnection(const TcpConnectionPtr& connection) override;

    virtual void OnConnectionError(const TcpConnectionPtr& connection) override;

private:

    std::unordered_map<std::string, GameConnectionPtr> connection_map_;
};