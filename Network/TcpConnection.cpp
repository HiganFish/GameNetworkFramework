#include "TcpConnection.h"

TcpConnection::TcpConnection(asio::ip::tcp::socket socket):
    socket_(std::move(socket)),
    on_new_data_func_(nullptr),
	on_conn_close_func_(nullptr)
{
}

void TcpConnection::AsyncWaitData()
{
    socket_.async_read_some(asio::buffer(buffer_.WriteBegin(), buffer_.WritableSize()), 
        [this](const asio::error_code& ec, size_t read_length)
        {
            if (ec)
            {
				if (asio::error::eof == ec || asio::error::connection_reset == ec)
				{
					if (on_conn_close_func_)
					{
						on_conn_close_func_(shared_from_this());
					}
				}
                return;
            }
            buffer_.AddWriteIndex(read_length);

            // 每个连接的buffer默认有4KB，暂且不进行扩容
            if (buffer_.WritableSize() == 0)
            {
                buffer_.AdjustBuffer();
            }

            if (on_new_data_func_)
            {
                on_new_data_func_(shared_from_this(), buffer_);
            }
            AsyncWaitData();
        });
}

void TcpConnection::AsyncSendData(const char* data, size_t length)
{
    asio::async_write(socket_, asio::buffer(data, length), 
        [length](const asio::error_code& ec, size_t write_length)
        {
            if (length != write_length)
            {
                std::cout << ec.message() << std::endl;
            }
        });
}

void TcpConnection::SetOnNewDataFunc(const OnNewDataFunc& func)
{
    on_new_data_func_ = func;
}

void TcpConnection::SetConnectionName(const std::string& connection_name)
{
    connection_name_ = connection_name;
}

const std::string& TcpConnection::GetConnectionName() const
{
    return connection_name_;
}

void TcpConnection::SetOnConnCloseFunc(const OnTcpConnectionCloseFunc& onConnCloseFunc)
{
	on_conn_close_func_ = onConnCloseFunc;
}
