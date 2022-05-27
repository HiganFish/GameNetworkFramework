#pragma once
#include <asio.cpp>

#if __cplusplus > 202002L
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;

#define awaitable_void awaitable<void>
#else
#define awaitable_void void
#define co_return return

#endif

#if defined(ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

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