#pragma once

#include "MKServer.h"

class GameServerExample : public MKServer
{
public:
	GameServerExample(const std::string& name, unsigned short port);

	void Ping(ROLE_ID role_id, const BaseMessagePtr& msg);
	void Control(ROLE_ID role_id, const BaseMessagePtr& msg);
	void EnterRoom(ROLE_ID role_id, const BaseMessagePtr& msg);

private:

	std::vector<ROLE_ID> role_ids;
};