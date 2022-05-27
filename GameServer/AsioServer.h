#pragma once

#include <cstdio>

#include "Network/NetworkCommon.h"
#include "Network/TcpConnection.h"

class AsioServer
{
public:

    AsioServer(const std::string& server_name, short port):
        started_(false),
        server_name_(server_name),
        port_(port),
        ep_(asio::ip::tcp::v4(), port_),
        acceptor_(io_context_, ep_),
        client_socket_(io_context_)
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

#if __cplusplus > 202002L
	        co_spawn(io_context_, Listener(), detached);
#else
			Listener();
#endif
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

private:
    bool started_;
    std::string server_name_;
    short port_;
    asio::io_context io_context_;
    asio::executor_work_guard<asio::io_context::executor_type> work_guard_{
        asio::make_work_guard(io_context_)};
    std::unordered_map<std::string, TcpConnectionPtr> tcp_connection_map_;

	asio::ip::tcp::endpoint ep_;
	asio::ip::tcp::acceptor acceptor_;

#if __cplusplus > 202002L
#else
	asio::ip::tcp::socket client_socket_;
#endif

	awaitable_void OnNewClient(asio::ip::tcp::socket socket)
    {
        try
        {
            std::ostringstream stream;
            stream << server_name_ << "#" << socket.remote_endpoint();
            // after move cannot be used
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

	awaitable_void Listener()
    {
#if __cplusplus > 202002L
		auto executor = co_await this_coro::executor;
        for (;;)
        {
	        asio::ip::tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
            co_spawn(socket.get_executor(), OnNewClient(std::move(socket)), detached);
        }
#else
		acceptor_.async_accept(client_socket_,
				[this](std::error_code ec)
		{
			if (!ec)
			{
				OnNewClient(std::move(client_socket_));
			}
			Listener();
		});
#endif
    }

};