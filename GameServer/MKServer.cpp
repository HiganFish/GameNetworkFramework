#include <memory>
#include "MKServer.h"

MKServer::MKServer(const std::string& name, short port,
	int recv_thread_num, int send_thread_num) :
	game_server_(name, port),
	recv_msg_dispatcher_(recv_thread_num),
	send_msg_dispatcher_(send_thread_num),
	started_(false)
{
	send_msg_dispatcher_.SetDefaultMsgCallback(
		[this](ROLE_ID role_id, const BaseMessagePtr& ptr)
		{
			TinyBuffer buffer;
			ptr->EncodeMessage(buffer);
			game_server_.SendMessageById(role_id, buffer.ReadBegin(), buffer.ReadableSize());
		});

	game_server_.SetOnNewMsgWithBufferAndIdFunc(
		[this](BaseMsgWithBufferAndIdPtr&& ptr)
		{
			new_pack_queue_.Push(std::move(ptr));
		});
}

void MKServer::Start()
{
	if (!started_)
	{
		started_ = true;

		decode_thread_ = std::thread(
			[this]()
			{
				while (started_)
				{
					BaseMsgWithBufferAndIdPtr ptr;
					bool has = new_pack_queue_.TryPop(ptr);
					if (!has)
					{
						continue;
					}
					auto msg = TransmitMessage(ptr);
					msg->base_message_ptr->DecodeMessageBody(ptr->body_buffer);
					recv_msg_dispatcher_.Push(std::move(msg));
				}
			});

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
		decode_thread_.join();
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
	send_msg_dispatcher_.Push(
		std::make_shared<BaseMsgWithRoleId>(role_id, msg_ptr));
}
