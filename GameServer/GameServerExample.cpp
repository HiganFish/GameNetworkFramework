#include "GameServerExample.h"
#include "Message/Messages.h"
#include "Utils/TimeUtils.h"

void GameServerExample::Ping(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	PingMessagePtr ping_ptr = CastBaseMsgTo<PingMessage>(msg);
	SendMessageByRoleId(role_id, msg);
}

void GameServerExample::Control(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	ControlMessagePtr control_ptr = CastBaseMsgTo<ControlMessage>(msg);
	if (control_ptr->move_direction == ControlMessage::MoveDirection::NONE)
	{
		{
			std::lock_guard guard(role_ids_mutex_);
			role_ids.push_back(role_id);
		}
		std::cout << fmt::format("add a new role: {}\r\n", role_id);
	}
	else
	{
		SendMessageByRoleIds(role_ids, msg);
	}
}

void GameServerExample::EnterRoom(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	std::cout << "EnterRoom\r\n";
}

void GameServerExample::OnRoleDisconnect(int32_t role_id)
{
	{
		std::lock_guard guard(role_ids_mutex_);
		auto iter =
				std::find(role_ids.begin(), role_ids.end(),role_id);
		if (iter == role_ids.end())
		{
			std::cout << "on role out some error" << std::endl;
		}
		else
		{
			role_ids.erase(iter);
		}
	}
	std::cout << fmt::format("remove a role: {}\r\n", role_id);
}


GameServerExample::GameServerExample(const std::string& name, unsigned short port):
	MKServer(name, port, 10, 10)
{
	SET_MSG_FUNC(MessageType::PING, Ping);
	SET_MSG_FUNC(MessageType::CONTROL, Control);
	SET_MSG_FUNC(MessageType::ENTER_ROOM, EnterRoom);

	SetOnRoleDisconnectFunc([this](ROLE_ID role_id)
	{
		OnRoleDisconnect(role_id);
	});
}