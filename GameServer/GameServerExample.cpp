#include "GameServerExample.h"
#include "Messages.h"

void GameServerExample::PlayerInit(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	std::cout << "PlayerInit\r\n";

	PlayerInitMessagePtr message = std::make_shared<PlayerInitMessage>();
	message->role_id = role_id / 10;
	SendMessageByRoleId(role_id, message);
}

void GameServerExample::Control(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	std::cout << "Control\r\n";
}

void GameServerExample::EnterRoom(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	std::cout << "EnterRoom\r\n";
}

GameServerExample::GameServerExample(const std::string& name, short port):
	MKServer(name, port, 1, 1)
{
	SET_MSG_FUNC(MessageType::PLAYER_INIT, PlayerInit);
	SET_MSG_FUNC(MessageType::CONTROL, Control);
	SET_MSG_FUNC(MessageType::ENTER_ROOM, EnterRoom);
}
