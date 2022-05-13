#include "GameServerExample.h"
#include "Messages.h"

void GameServerExample::Ping(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	std::cout << "Ping\r\n";
	SendMessageByRoleId(role_id, msg);
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
	SET_MSG_FUNC(MessageType::PING, Ping);
	SET_MSG_FUNC(MessageType::CONTROL, Control);
	SET_MSG_FUNC(MessageType::ENTER_ROOM, EnterRoom);
}
