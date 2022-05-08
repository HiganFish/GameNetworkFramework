#include "GameConnection.h"

GameConnection::GameConnection(const TcpConnectionPtr& connection):
	tcp_connection_(connection),
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

void GameConnection::SetOnNewMsgWithIdFunc(const OnNewMsgWithIdFunc& func)
{
	on_new_msg_with_id_func_ = func;
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

void GameConnection::OnNewData(const TcpConnectionPtr& connection, Buffer& buffer)
{
	bool data_enought = true;
	bool error = false;
	while (data_enought && !error)
	{
		auto message_ptr = SpawnNewMessage<BaseMessage>();
		auto [ok, length] = message_ptr->DecodeMessage(buffer);
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
			// std::cout << std::format("{} - a new message, {}{}", connection_name_,
				// message_ptr->DebugMessage(), CRLF);

			if (on_new_msg_with_id_func_)
			{
				on_new_msg_with_id_func_(
					std::make_shared<BaseMsgWithRoleId>(role_id_, std::move(message_ptr)));
			}
		}
	}
}
