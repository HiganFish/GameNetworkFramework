#include "GameConnection.h"

GameConnection::GameConnection(const TcpConnectionPtr& connection):
	tcp_connection_(connection),
	connection_name_(tcp_connection_->GetConnectionName())
{
	tcp_connection_->SetOnNewMessageCallback(
		[this](const BaseMessagePtr& message_ptr)
		{
			OnNewMessage(message_ptr);
		});

	tcp_connection_->AsyncWaitData();
}

void GameConnection::OnNewMessage(const BaseMessagePtr& message_ptr)
{
	std::cout << std::format("{} - a new message, {}", connection_name_,
		message_ptr->DebugMessage()) << std::endl;
}
