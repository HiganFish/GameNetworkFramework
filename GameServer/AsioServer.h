#pragma once

#include <cstdio>

#include "NetworkCommon.h"
#include "TcpConnection.h"

class AsioServer
{
public:

    AsioServer(const std::string& server_name, short port):
        started_(false),
        server_name_(server_name),
        port_(port)
    {
    }
    virtual ~AsioServer()
    {}
    void Start()
    {
        try
        {
            if (started_)
            {
                return;
            }
            started_ = true;
            
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

    void Stop()
    {
        work_guard_.reset();
        io_context_.stop();
    }

    virtual void OnNewTcpConnection(const TcpConnectionPtr& connection)
    {
    }

    virtual void OnConnectionError(const TcpConnectionPtr& connection)
    {
    }

private:
    bool started_;
    std::string server_name_;
    short port_;
    asio::io_context io_context_;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_{
        asio::make_work_guard(io_context_)};
    std::unordered_map<std::string, TcpConnectionPtr> tcp_connection_map_;

    awaitable<void> OnNewClient(tcp::socket socket)
    {
        try
        {
            std::ostringstream stream;
            stream << server_name_ << "#" << socket.remote_endpoint();
            // 注意move后 socket不能再次使用
            auto connection_ptr = std::make_shared<TcpConnection>(std::move(socket));

            std::string connection_name = stream.str();
            connection_ptr->SetConnectionName(connection_name);
            tcp_connection_map_[connection_name] = connection_ptr;

            OnNewTcpConnection(connection_ptr);
        }
        catch (std::exception& e)
        {
            std::printf("Exception: %s\n", e.what());
        }
        co_return;
    }

    awaitable<void> Listener()
    {
        auto executor = co_await this_coro::executor;
        tcp::endpoint ep(tcp::v4(), port_);
        tcp::acceptor acceptor(executor, ep);
        for (;;)
        {
            tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
            co_spawn(socket.get_executor(), OnNewClient(std::move(socket)), detached);
        }
    }

};