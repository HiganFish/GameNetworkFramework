#pragma once

#define _WIN32_WINNT 0x0601

#include <asio.hpp>
#include <cstdio>

#include "TcpConnection.h"

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

class AsioServer
{
public:

    AsioServer(const std::string& server_name, short port):
        server_name_(server_name),
        port_(port)
    {
    }

    void Start()
    {
        try
        {
            asio::io_context io_context_;

            asio::signal_set signals(io_context_, SIGINT, SIGTERM);
            signals.async_wait([&](auto, auto) { io_context_.stop(); });

            co_spawn(io_context_, Listener(), detached);

            io_context_.run();
        }
        catch (std::exception& e)
        {
            std::printf("Exception: %s\n", e.what());
        }
    }

    virtual void OnNewTcpConnection(const TcpConnectionPtr& connection)
    {
    }

    virtual void OnConnectionError(const TcpConnectionPtr& connection)
    {
    }

private:
    
    std::string server_name_;
    short port_;
    asio::io_context io_context_;

    void OnNewClient(tcp::socket socket)
    {
        try
        {
            std::ostringstream stream;
            stream << server_name_ << "#" << socket.remote_endpoint();
            // 注意move后 socket不能再次使用
            auto connection_ptr = std::make_shared<TcpConnection>(std::move(socket));
            connection_ptr->SetConnectionName(stream.str());
      
            OnNewTcpConnection(connection_ptr);
        }
        catch (std::exception& e)
        {
            std::printf("echo Exception: %s\n", e.what());
        }
    }

    awaitable<void> Listener()
    {
        auto executor = co_await this_coro::executor;
        tcp::endpoint ep(tcp::v4(), port_);
        tcp::acceptor acceptor(executor, ep);
        for (;;)
        {
            tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
            OnNewClient(std::move(socket));
        }
    }

};