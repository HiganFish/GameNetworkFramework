#include "GameServer.h"

GameServer::GameServer(const std::string& server_name, short port):
	AsioServer(server_name, port)
{
}

void GameServer::OnNewTcpConnection(const TcpConnectionPtr& connection)
{
	auto game_connection_ptr = std::make_shared<GameConnection>(connection);
	connection_map_.insert({ game_connection_ptr->GetConnectionName(), game_connection_ptr });
}


void GameServer::OnConnectionError(const TcpConnectionPtr& connection)
{
}
