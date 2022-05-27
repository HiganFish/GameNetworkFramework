#pragma once

#include "MKServer.h"

class GameServerExample : public MKServer
{
public:
	GameServerExample(const std::string& name, unsigned short port);
	~GameServerExample();

	void Ping(ROLE_ID role_id, const BaseMessagePtr& msg);
	void Control(ROLE_ID role_id, const BaseMessagePtr& msg);
	void EnterRoom(ROLE_ID role_id, const BaseMessagePtr& msg);
	void OnRoleDisconnect(ROLE_ID role_id);
private:

	std::mutex role_ids_mutex_;
	std::vector<ROLE_ID> role_ids;

private:

	bool running_;
	std::thread main_thread_;

	const int LOGIC_FPS = 10;
	const std::chrono::milliseconds LOGIC_FRAME_MS{1000 / LOGIC_FPS};
	std::chrono::time_point<std::chrono::steady_clock> last_frame_begin_time;

	std::vector<BaseMessagePtr> control_msg_;
	std::vector<BaseMessagePtr> send_msg_;
	std::mutex control_msg_lock_;
	uint32_t frame_counter_;

	struct FrameInput
	{
		uint32_t frame_number;
		std::vector<BaseMessagePtr> send_msg;
	};
	std::vector<FrameInput> frame_input_record_vec_;

	void Update(std::chrono::milliseconds delta_time);

	void RePlay(ROLE_ID role_id);
};