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

struct ControlMessage;
using ControlMessagePtr = std::shared_ptr<ControlMessage>;


class GameConnection;
using GameConnectionPtr = std::shared_ptr<GameConnection>;


using OnNewMsgWithIdFunc = std::function<void(BaseMsgWithRoleIdPtr&&)>;
using TryGetMessageFunc = std::function<bool(BaseMessagePtr&)>;


class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using OnNewMessageFunc = std::function<void(BaseMessagePtr&&)>;
using OnNewDataFunc = std::function<void(const TcpConnectionPtr&, Buffer&)>;
