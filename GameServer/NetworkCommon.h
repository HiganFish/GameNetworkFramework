#pragma once

#include <memory>
#include <functional>

#define CRLF "\r\n"

typedef uint32_t ROLE_ID;

constexpr int TINY_BUFFER_DEFAULT_BUFFER_SIZE = 64;

struct BaseMessage;
using BaseMessagePtr = std::shared_ptr<BaseMessage>;

struct BaseMsgWithRoleId;
using BaseMsgWithRoleIdPtr = std::shared_ptr<BaseMsgWithRoleId>;

struct BaseMsgWithBuffer;
using BaseMsgWithBufferPtr = std::shared_ptr<BaseMsgWithBuffer>;

struct BaseMsgWithBufferAndId;
using BaseMsgWithBufferAndIdPtr = std::shared_ptr<BaseMsgWithBufferAndId>;

struct ControlMessage;
using ControlMessagePtr = std::shared_ptr<ControlMessage>;

struct PlayerInitMessage;
using PlayerInitMessagePtr = std::shared_ptr<PlayerInitMessage>;

class GameConnection;
using GameConnectionPtr = std::shared_ptr<GameConnection>;


using OnNewMsgWithBufferAndIdFunc = std::function<void(BaseMsgWithBufferAndIdPtr&&)>;
using TryGetMessageFunc = std::function<bool(BaseMessagePtr&)>;

using RegisterRoleIdConnFunc = std::function<void(ROLE_ID role_id, const GameConnectionPtr&)>;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using OnNewMessageFunc = std::function<void(BaseMessagePtr&&)>;
using OnNewDataFunc = std::function<void(const TcpConnectionPtr&, Buffer&)>;
