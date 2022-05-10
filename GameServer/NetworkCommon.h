#pragma once

#include <memory>
#include <functional>
#include "BaseMessage.h"
#include "Common.h"

class GameConnection;
using GameConnectionPtr = std::shared_ptr<GameConnection>;

using OnNewMsgWithBufferAndIdFunc = std::function<void(BaseMsgWithBufferAndIdPtr&&)>;
using TryGetMessageFunc = std::function<bool(BaseMessagePtr&)>;

using RegisterRoleIdConnFunc = std::function<void(ROLE_ID role_id, const GameConnectionPtr&)>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using OnNewMessageFunc = std::function<void(BaseMessagePtr&&)>;
using OnNewDataFunc = std::function<void(const TcpConnectionPtr&, Buffer&)>;
