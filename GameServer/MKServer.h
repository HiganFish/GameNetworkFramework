#pragma once
#include <thread>
#include "GameServer.h"
#include "MsgDispatcher.h"

class MKServer
{
public:
	
	MKServer(const std::string& name, short port, 
		int recv_thread_num, int send_thread_num);

	void Start();

	void Stop();

private:

	GameServer game_server_;
	ThreadSafeQueue<BaseMsgWithBufferAndIdPtr> new_pack_queue_;
	MsgDispatcher recv_msg_dispatcher_;
	MsgDispatcher send_msg_dispatcher_;
	std::thread decode_thread_;

	bool started_;
};

