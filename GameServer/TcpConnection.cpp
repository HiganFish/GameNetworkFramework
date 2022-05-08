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
                std::cout << "read data error, " << ec.message() << std::endl;
                // TODO handle error close connection
                return;
            }
            buffer_.AddWriteIndex(read_length);

            // 每个连接的buffer默认有4KB，暂且不进行扩容
            if (buffer_.WritableSize() == 0)
            {
                buffer_.AdjustBuffer();
            }

            bool enought = true;
            while (enought)
            {
                auto [ok, decode_size] = DecodeBufferData();
                if (!ok)
                {
                    // TODO handle error close connection
                    return;
                }
                if (decode_size == 0)
                {
                    enought = false;
                }
            }
            AsyncWaitData();
        });
}

std::pair<bool, size_t> TcpConnection::DecodeBufferData()
{
    auto message_ptr = SpawnNewMessage<BaseMessage>();
    auto [ok, length] = message_ptr->DecodeMessage(buffer_);
    if (!ok)
    {
        std::cout << "decode error" << std::endl; 
        return {false, 0};
    }

    if (length > 0 && on_new_message_)
    {
        on_new_message_(message_ptr);
    }

    return { true, length };
}

