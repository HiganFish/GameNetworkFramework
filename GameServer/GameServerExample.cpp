#include "GameServerExample.h"
#include "Message/Messages.h"
#include "Utils/TimeUtils.h"

void GameServerExample::Ping(ROLE_ID role_id, BaseMessagePtr msg)
{
	PingMessagePtr ping_ptr = CastBaseMsgTo<PingMessage>(msg);
//	static int count = 0;
//	count++;
//	printf("%lld\r\n", NOW_MS - ping_ptr->timestamp);
//	if (count >= 100)
//	{
//		printf("---\r\n");
//		count = 0;
//	}
	SendMessageByRoleId(role_id, msg);
}

void GameServerExample::Control(ROLE_ID role_id, BaseMessagePtr msg)
{
	std::cout << "Control\r\n";
}

void GameServerExample::EnterRoom(ROLE_ID role_id, BaseMessagePtr msg)
{
	std::cout << "EnterRoom\r\n";
}

GameServerExample::GameServerExample(const std::string& name, unsigned short port):
	MKServer(name, port, 10, 10)
{
	SET_MSG_FUNC(MessageType::PING, Ping);
	SET_MSG_FUNC(MessageType::CONTROL, Control);
	SET_MSG_FUNC(MessageType::ENTER_ROOM, EnterRoom);
}
