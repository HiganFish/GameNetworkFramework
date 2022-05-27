#include "GameServer.h"

GameServer::GameServer(const std::string& server_name, short port):
	AsioServer(server_name, port)
{
}

void GameServer::OnNewTcpConnection(const TcpConnectionPtr& connection)
{
	auto game_connection_ptr = std::make_shared<GameConnection>(connection);
	game_connection_ptr->SetOnNewMsgWithBufferFunc(on_new_msg_with_buffer_func_);
	game_connection_ptr->SetRegisterFunc([this](auto&& PH1, auto&& PH2) {AddConnToRoleMap(PH1, PH2); });
	{
		std::unique_lock guard(conn_map_mutex_);
		game_connection_map_.insert({ game_connection_ptr->GetConnectionName(), game_connection_ptr });
	}
	game_connection_ptr->SetOnGameConnCloseFunc([this](auto&& PH1)
	{
		OnConnectionClose(PH1);
	});

	game_connection_ptr->StartRecvData();
}

void GameServer::OnConnectionClose(const GameConnectionPtr & connection)
{
	auto role_id = connection->GetRoleId();
	{
		std::unique_lock guard(role_id_map_mutex_);
		auto iter = role_id_conn_map_.find(role_id);
		if (iter == role_id_conn_map_.end())
		{
			std::cout << "connection close error, role id not exist" << std::endl;
			return;
		}
		else
		{
			role_id_conn_map_.erase(iter);
		}
	}
	{
		std::unique_lock guard(conn_map_mutex_);
		auto iter = game_connection_map_.find(connection->GetConnectionName());
		if (iter == game_connection_map_.end())
		{
			std::cout << "connection close error, game conn not exist" << std::endl;
			return;
		}
		else
		{
			game_connection_map_.erase(iter);
		}
	}
	if (on_role_disconnct_func_)
	{
		on_role_disconnct_func_(role_id);
	}
	std::cout << "game conn: " << connection->GetConnectionName() <<
		" closed, role_id: " << role_id << std::endl;
}

void GameServer::SetTryGetMessageFunc(const TryGetMessageFunc& func)
{
	try_get_message_func_ = func;
}

void GameServer::SetOnNewMsgWithBufferFunc(const OnNewMsgWithBufferFunc& func)
{
	on_new_msg_with_buffer_func_ = func;
}

void GameServer::SendMessageById(ROLE_ID role_id, const char* data, size_t length)
{
	std::shared_lock guard(role_id_map_mutex_);
	auto iter = role_id_conn_map_.find(role_id);
	if (iter != role_id_conn_map_.end())
	{
		iter->second->AsyncSendData(data, length);
	}
	else
	{
		std::cout << fmt::format("unknown role_id: {}\r\n", role_id);
	}
}

void GameServer::AddConnToRoleMap(ROLE_ID role_id, const GameConnectionPtr& conn)
{
	std::unique_lock guard(role_id_map_mutex_);
	role_id_conn_map_[role_id] = conn;
}

void GameServer::SetOnRoleDisconnectFunc(const OnRoleDisconnectFunc& func)
{
	on_role_disconnct_func_ = func;
}