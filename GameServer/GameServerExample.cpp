#include <numeric>
#include "GameServerExample.h"
#include "Message/Messages.h"
#include "Utils/TimeUtils.h"
#include "Utils/FormatUtils.h"

void GameServerExample::Ping(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	PingMessagePtr ping_ptr = CastBaseMsgTo<PingMessage>(msg);
	SendMessageByRoleId(role_id, msg);
}

void GameServerExample::Control(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	ControlMessagePtr control_ptr = CastBaseMsgTo<ControlMessage>(msg);
	// SendMessageByRoleIds(role_ids, msg);
	std::lock_guard<std::mutex> guard(control_msg_lock_);
	control_msg_.push_back(msg);
}

void GameServerExample::EnterRoom(ROLE_ID role_id, const BaseMessagePtr& msg)
{
	std::cout << "EnterRoom\r\n";
	auto enter_msg = CastBaseMsgTo<EnterRoomMessage>(msg);
	enter_msg->result = 0;

	{
		std::lock_guard guard(role_ids_mutex_);
		role_ids.push_back(role_id);
	}
	LOG_INFO << "add a new role: " << role_id;
	if (role_id == 2)
	{
		GameStartMessagePtr start_msg_ptr = SpawnNewMessage<GameStartMessage>();
		start_msg_ptr->timestamp = NOW_MS;
		frame_counter_ = 0;
		SendMessageByRoleIds(role_ids, start_msg_ptr);

		LOG_INFO << "game start, player: " << role_ids;
	}
	if (role_id == 3)
	{
		GameStartMessagePtr start_msg_ptr = SpawnNewMessage<GameStartMessage>();
		start_msg_ptr->timestamp = NOW_MS;
		SendMessageByRoleId(role_id, start_msg_ptr);
		LOG_INFO << "replay to " << role_id;
		RePlay(role_id);
	}

	SendMessageByRoleId(role_id, msg);
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
	LOG_INFO << "remove a role: " << role_id;
}


GameServerExample::GameServerExample(const std::string& name, unsigned short port):
	MKServer(name, port, 10, 10),
	running_(true),
	frame_counter_(true),
	frame_input_record_vec_()
{
	SET_MSG_FUNC(MessageType::PING, Ping);
	SET_MSG_FUNC(MessageType::CONTROL, Control);
	SET_MSG_FUNC(MessageType::ENTER_ROOM, EnterRoom);

	SetOnRoleDisconnectFunc([this](ROLE_ID role_id)
	{
		OnRoleDisconnect(role_id);
	});

	last_frame_begin_time = std::chrono::steady_clock::now();
	main_thread_ = std::thread(
			[this]()
			{
				while (running_)
				{
					auto frame_begin_time = std::chrono::steady_clock::now();
					auto delta_time = frame_begin_time - last_frame_begin_time;
					last_frame_begin_time = frame_begin_time;

					Update(std::chrono::duration_cast<std::chrono::milliseconds>(delta_time));

					auto frame_end_time = std::chrono::steady_clock::now();

					auto frame_cost = frame_end_time - frame_begin_time;
					std::this_thread::sleep_for(LOGIC_FRAME_MS - frame_cost);
				}
			});
}

GameServerExample::~GameServerExample()
{
	running_ = false;
	main_thread_.join();
}

void GameServerExample::Update(std::chrono::milliseconds delta_time)
{
	frame_counter_++;
	{
		std::lock_guard guard(control_msg_lock_);
		send_msg_.swap(control_msg_);
	}
	frame_input_record_vec_.push_back({frame_counter_, send_msg_});

	for (auto& msg : send_msg_)
	{
		auto cont_msg_ptr = CastBaseMsgTo<ControlMessage>(msg);
		cont_msg_ptr->tick = frame_counter_;
		SendMessageByRoleIds(role_ids, cont_msg_ptr);
	}
	send_msg_.clear();
}

void GameServerExample::RePlay(ROLE_ID role_id)
{
	for (FrameInput& frame_input : frame_input_record_vec_)
	{
		for (auto& msg : frame_input.send_msg)
		{
			SendMessageByRoleId(role_id, msg);
		}
	}
}
