#include "GameServer.h"

GameServer::GameServer(const std::string& server_name, short port):
	AsioServer(server_name, port)
{
}

void GameServer::OnNewTcpConnection(const TcpConnectionPtr& connection)
{
	auto game_connection_ptr = std::make_shared<GameConnection>(connection);
	game_connection_ptr->SetOnNewMsgWithIdFunc(on_new_msg_with_id_func_);
	game_connection_map_.insert({ game_connection_ptr->GetConnectionName(), game_connection_ptr });

	game_connection_ptr->StartRecvData();
}

void GameServer::OnConnectionError(const TcpConnectionPtr& connection)
{
}

void GameServer::SetTryGetMessageFunc(const TryGetMessageFunc& func)
{
	try_get_message_func_ = func;
}

void GameServer::SetOnNewMsgWithIdFunc(const OnNewMsgWithIdFunc& func)
{
	on_new_msg_with_id_func_ = func;
}

void GameServer::SendMessageById(ROLE_ID role_id, const char* data, size_t length)
{
	// TODO temp code

	for (auto [connection_name, connection_ptr] : game_connection_map_)
	{
		connection_ptr->AsyncSendData(data, length);
	}
}
