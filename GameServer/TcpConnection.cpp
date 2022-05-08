#include "TcpConnection.h"

TcpConnection::TcpConnection(asio::ip::tcp::socket socket):
    socket_(std::move(socket))
{
}

void TcpConnection::AsyncWaitData()
{
    socket_.async_read_some(asio::buffer(buffer_.WriteBegin(), buffer_.WritableSize()), 
        [this](const asio::error_code& ec, size_t read_length)
        {
            if (ec)
            {
                // std::cout << "read data error, " << ec.message() << std::endl;
                // TODO handle error close connection
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
            assert(length == write_length);
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
