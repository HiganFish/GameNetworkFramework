#pragma once
#define _WIN32_WINNT 0x0601
#include <asio.hpp>

using asio::ip::tcp;
using asio::awaitable;
using asio::co_spawn;
using asio::detached;
using asio::use_awaitable;
namespace this_coro = asio::this_coro;

#if defined(ASIO_ENABLE_HANDLER_TRACKING)
# define use_awaitable \
  asio::use_awaitable_t(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif

#include <memory>
#include <functional>
#include "BaseMessage.h"


class GameConnection;
using GameConnectionPtr = std::shared_ptr<GameConnection>;

using OnNewMsgWithBufferFunc = std::function<void(BaseMsgWithBufferPtr&&)>;
using TryGetMessageFunc = std::function<bool(BaseMessagePtr&)>;

using RegisterRoleIdConnFunc = std::function<void(ROLE_ID role_id, const GameConnectionPtr&)>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using OnNewMessageFunc = std::function<void(BaseMessagePtr&&)>;
using OnNewDataFunc = std::function<void(const TcpConnectionPtr&, Buffer&)>;
