#include <memory>
#include "MKServer.h"

MKServer::MKServer(const std::string& name, unsigned short port,
	int recv_thread_num, int send_thread_num) :
	game_server_(name, port),
	recv_msg_dispatcher_(recv_thread_num),
	send_msg_dispatcher_(send_thread_num),
	started_(false)
{
	send_msg_dispatcher_.SetDefaultMsgCallback(
		[this](ROLE_ID role_id, BaseMessagePtr ptr)
		{
			TinyBuffer buffer;
			ptr->EncodeMessage(buffer);
			game_server_.SendMessageById(role_id, buffer.ReadBegin(), buffer.ReadableSize());
		});

	game_server_.SetOnNewMsgWithBufferFunc(
		[this](auto&& ptr)
		{
			// new_pack_queue_.Push(std::move(ptr));
			auto msg = TransmitMessage(ptr);
			recv_msg_dispatcher_.Push(msg->role_id, msg);
		});
}

void MKServer::Start()
{
	if (!started_)
	{
		started_ = true;

		recv_msg_dispatcher_.Start();
		send_msg_dispatcher_.Start();
		game_server_.Start();
	}
}

void MKServer::Stop()
{
	if (started_)
	{
		started_ = false;
		recv_msg_dispatcher_.Stop();
		send_msg_dispatcher_.Stop();
		// decode_thread_.join();
		game_server_.Stop();
	}
}

void MKServer::SetMsgCallback(MessageType type, 
	const MsgDispatcher::MsgCallback& callback)
{
	recv_msg_dispatcher_.SetMsgCallback(type, callback);
}

void MKServer::SendMessageByRoleId(ROLE_ID role_id, const BaseMessagePtr& msg_ptr)
{
	send_msg_dispatcher_.Push(role_id, msg_ptr);
}

void MKServer::SendMessageByRoleIds(const std::vector<ROLE_ID>& role_ids, const BaseMessagePtr& msg_ptr)
{
	send_msg_dispatcher_.Push(role_ids, msg_ptr);
}

void MKServer::SetOnRoleDisconnectFunc(const OnRoleDisconnectFunc& func)
{
	game_server_.SetOnRoleDisconnectFunc(func);
}
