#include "GameConnection.h"
#include "Message/Messages.h"

GameConnection::GameConnection(const TcpConnectionPtr& connection):
	tcp_connection_(connection),
	register_func_(nullptr),
	role_id_(0),
	connection_name_(tcp_connection_->GetConnectionName()),
	recving_data_(false),
	on_new_msg_with_buf_func_(nullptr)
{
	tcp_connection_->SetOnNewDataFunc(
		[this](auto&& PH1, auto&& PH2)
		{
			OnNewData(PH1, PH2);
		});
}
const std::string& GameConnection::GetConnectionName()
{
	return connection_name_;
}

void GameConnection::SetOnNewMsgWithBufferFunc(const OnNewMsgWithBufferFunc& func)
{
	on_new_msg_with_buf_func_ = func;
}

void GameConnection::AsyncSendData(const char* data, size_t length)
{
	tcp_connection_->AsyncSendData(data, length);
}

void GameConnection::StartRecvData()
{
	if (!recving_data_)
	{
		recving_data_ = true;
		tcp_connection_->AsyncWaitData();
	}
}

void GameConnection::SetRegisterFunc(const RegisterRoleIdConnFunc& func)
{
	register_func_ = func;
}

void GameConnection::OnNewData(const TcpConnectionPtr& connection, Buffer& buffer)
{
	bool data_enought = true;
	bool error = false;
	while (data_enought && !error)
	{
		auto message_ptr = std::make_shared<BaseMsgWithBuffer>();
		auto base_message_ptr = SpawnNewMessage<BaseMessage>();
		message_ptr->base_message_ptr = base_message_ptr;
		uint32_t body_size;
		auto [ok, length] = base_message_ptr->DecodeMessageHeader(buffer, &body_size);
		if (!ok)
		{
			error = true;
			std::cout << "decode error" << std::endl;
			// TODO ERROR handle
		}
		if (length == 0)
		{
			data_enought = false;
		}
		else
		{
			if (role_id_ == 0)
			{
				role_id_ = base_message_ptr->role_id;
				if (register_func_)
				{
					register_func_(role_id_, shared_from_this());
				}
			}
			// TODO 关闭连接
			// assert(role_id_ == base_message_ptr->role_id);

			assert(buffer.ReadableSize() >= body_size);
			message_ptr->body_buffer.AppendData(buffer.ReadBegin(), body_size);
			buffer.AddReadIndex(body_size);

			if (on_new_msg_with_buf_func_)
			{
				on_new_msg_with_buf_func_(std::move(message_ptr));
			}
		}
	}
}
