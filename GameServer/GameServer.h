#pragma once

#include <shared_mutex>
#include "AsioServer.h"
#include "GameConnection.h"

class GameServer : public AsioServer
{
public:

    GameServer(const std::string& server_name, short port);

    virtual void OnNewTcpConnection(const TcpConnectionPtr& connection) override;

    virtual void OnConnectionError(const TcpConnectionPtr& connection) override;

    void SetTryGetMessageFunc(const TryGetMessageFunc& func);

    void SetOnNewMsgWithBufferFunc(const OnNewMsgWithBufferFunc& func);

    void SendMessageById(ROLE_ID role_id, const char* data, size_t length);

    void AddConnToRoleMap(ROLE_ID role_id, const GameConnectionPtr& conn);

private:
    mutable std::shared_mutex conn_map_mutex_;
    mutable std::shared_mutex role_id_map_mutex_;
    std::unordered_map<std::string, GameConnectionPtr> game_connection_map_;
    std::unordered_map<ROLE_ID, GameConnectionPtr> role_id_conn_map_;

    TryGetMessageFunc try_get_message_func_;

    OnNewMsgWithBufferFunc on_new_msg_with_buffer_func_;
};