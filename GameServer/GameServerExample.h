#pragma once

#include "MKServer.h"

class GameServerExample : public MKServer
{
public:
	GameServerExample(const std::string& name, unsigned short port);

	void Ping(ROLE_ID role_id, BaseMessagePtr msg);
	void Control(ROLE_ID role_id, BaseMessagePtr msg);
	void EnterRoom(ROLE_ID role_id, BaseMessagePtr msg);

};