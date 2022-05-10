#include "GameConnection.h"
#include "PlayerInitMessage.h"

GameConnection::GameConnection(const TcpConnectionPtr& connection):
	tcp_connection_(connection),
	register_func_(nullptr),
	role_id_(0),
	connection_name_(tcp_connection_->GetConnectionName()),
	recving_data_(false)
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

void GameConnection::SetOnNewMsgWithBufferAndIdFunc(const OnNewMsgWithBufferAndIdFunc& func)
{
	on_new_msg_with_buf_and_id_func_ = func;
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
		auto message_ptr = std::make_shared<BaseMsgWithBufferAndId>();
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
			if (base_message_ptr->message_type == MessageType::PLAYER_INIT)
			{
				PlayerInitMessage msg;
				msg.DecodeMsgBody(buffer.ReadBegin(), body_size);
				role_id_ = msg.role_id;
				if (register_func_)
				{
					register_func_(role_id_, shared_from_this());
				}
			}

			message_ptr->role_id = role_id_;
			assert(buffer.ReadableSize() >= body_size);
			message_ptr->body_buffer.AppendData(buffer.ReadBegin(), body_size);
			buffer.AddReadIndex(body_size);
			

			// std::cout << std::format("{} - a new message, {}{}", connection_name_,
				// message_ptr->DebugMessage(), CRLF);

			if (on_new_msg_with_buf_and_id_func_)
			{
				on_new_msg_with_buf_and_id_func_(std::move(message_ptr));
			}
		}
	}
}
