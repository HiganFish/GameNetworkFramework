#pragma once
#include <asio.cpp>

#include <memory>
#include <functional>
#include "Message/BaseMessage.h"


class GameConnection;
using GameConnectionPtr = std::shared_ptr<GameConnection>;

using OnNewMsgWithBufferFunc = std::function<void(BaseMsgWithBufferPtr&&)>;
using TryGetMessageFunc = std::function<bool(BaseMessagePtr&)>;

using RegisterRoleIdConnFunc = std::function<void(ROLE_ID role_id, const GameConnectionPtr&)>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using OnNewMessageFunc = std::function<void(BaseMessagePtr&&)>;
using OnNewDataFunc = std::function<void(const TcpConnectionPtr&, Buffer&)>;
using OnTcpConnectionCloseFunc = std::function<void(const TcpConnectionPtr&)>;
using OnGameConnectionCloseFunc = std::function<void(const GameConnectionPtr&)>;
using OnRoleDisconnectFunc = std::function<void(ROLE_ID)>;