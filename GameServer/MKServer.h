#pragma once
#include <thread>
#include "GameServer.h"
#include "Network/MsgDispatcher.h"

#define SET_MSG_FUNC(type, func_name) \
SetMsgCallback(type, [this](auto&& PH1, auto&& PH2) {func_name(PH1, PH2); });

class MKServer
{
public:
	
	MKServer(const std::string& name, unsigned short port,
		int recv_thread_num, int send_thread_num);
	MKServer(const MKServer&) = delete;
	MKServer& operator=(const MKServer&) = delete;

	void Start();

	void Stop();

	void SetMsgCallback(MessageType type, const MsgDispatcher::MsgCallback& callback);

	void SendMessageByRoleId(ROLE_ID role_id, const BaseMessagePtr& msg_ptr);
	void SendMessageByRoleIds(const std::vector<ROLE_ID>& role_ids, const BaseMessagePtr& msg_ptr);

	void SetOnRoleDisconnectFunc(const OnRoleDisconnectFunc& func);
private:

	GameServer game_server_;
	ThreadSafeQueue<BaseMsgWithBufferPtr> new_pack_queue_;
	MsgDispatcher recv_msg_dispatcher_;
	MsgDispatcher send_msg_dispatcher_;
	std::thread decode_thread_;

	bool started_;
};

