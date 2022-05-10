#include "pch.h"
#include <thread>
#include "ControlMessage.h"
#include "GameServer.h"
#include "ThreadSafeQueue.h"
#include "MsgDispatcher.h"
#include "Messages.h"

TEST(Message, MessageCodec)
{
	Buffer buffer;
	
	ControlMessage message;
	message.player_id = 101010;
	message.tick = 1234;
	message.control_type = ControlMessage::ControlType::DOWN;

	std::cout << message.DebugMessage() << std::endl;

	message.EncodeMessage(buffer);

	BaseMessagePtr ptr = SpawnNewMessage<BaseMessage>();
	ptr->DecodeMessageHeader(buffer);

	if (ptr->message_type == MessageType::CONTROL)
	{
		ControlMessage new_message(std::move(*ptr));
		new_message.DecodeMessageBody(buffer);
		assert(buffer.ReadableSize() == 0);
		std::cout << new_message.DebugMessage() << std::endl;
	}
}

static int PACK_SUM = 50;
static int THREAD_NUM = 10;

int FooMain(GameServer& game_server, bool& running)
{
	// bool running = true;
	// GameServer game_server("TestServer", 40000);

	ThreadSafeQueue<BaseMsgWithBufferAndIdPtr> new_pack_queue;
	MsgDispatcher recv_msg_dispatcher_(1);
	recv_msg_dispatcher_.Start();
	MsgDispatcher send_msg_dispatcher_(1);
	send_msg_dispatcher_.Start();
	recv_msg_dispatcher_.SetMsgCallback(MessageType::PLAYER_INIT,
		[&send_msg_dispatcher_](ROLE_ID role_id, const BaseMessagePtr& ptr)
		{
			PlayerInitMessagePtr message = std::make_shared<PlayerInitMessage>();
			message->role_id = role_id  / 10;
			auto msg = std::make_shared<BaseMsgWithRoleId>();
			msg->role_id = role_id;
			msg->base_message_ptr = message;
			send_msg_dispatcher_.Push(std::move(msg));
		});
	send_msg_dispatcher_.SetDefaultMsgCallback(
		[&send_msg_dispatcher_, &game_server](ROLE_ID role_id, const BaseMessagePtr& ptr)
		{
			TinyBuffer buffer;
			ptr->EncodeMessage(buffer);
			game_server.SendMessageById(role_id, buffer.ReadBegin(), buffer.ReadableSize());
		});
	send_msg_dispatcher_.SetMsgCallback(MessageType::PLAYER_INIT,
		[&send_msg_dispatcher_, &game_server](ROLE_ID role_id, const BaseMessagePtr& ptr)
		{
			TinyBuffer buffer;
			ptr->EncodeMessage(buffer);
			game_server.SendMessageById(role_id, buffer.ReadBegin(), buffer.ReadableSize());
		});

	game_server.SetOnNewMsgWithBufferAndIdFunc(
		[&new_pack_queue](BaseMsgWithBufferAndIdPtr&& ptr)
		{
			new_pack_queue.Push(std::move(ptr));
		});
	std::thread decode_thread(
		[&running, &new_pack_queue, &recv_msg_dispatcher_]()
		{
			while (running)
			{
				BaseMsgWithBufferAndIdPtr ptr;
				bool has = new_pack_queue.TryPop(ptr);
				if (!has)
				{
					continue;
				}

				auto msg = std::make_shared<BaseMsgWithRoleId>();
				msg->role_id = ptr->role_id;

				switch (ptr->base_message_ptr->message_type)
				{
				case MessageType::CONTROL:
				{
					msg->base_message_ptr =
						std::make_shared<ControlMessage>(std::move(*ptr->base_message_ptr));
					

					// BaseMessagePtr message_ptr = SpawnNewMessage<ControlMessage>(*ptr->base_message_ptr);
					// std::cout << std::format("{}{}", message_ptr->DebugMessage(), CRLF);

					/*static std::atomic_int number = 0;
					static std::chrono::time_point begin_time = std::chrono::system_clock::now();
					static std::chrono::time_point end_time = std::chrono::system_clock::now();
					number++;
					if (number == 1)
					{
						begin_time = std::chrono::system_clock::now();
					}
					if (number == PACK_SUM)
					{
						end_time = std::chrono::system_clock::now();
						std::cout << "cost " <<
							std::chrono::duration_cast<std::chrono::milliseconds>(end_time - begin_time).count() <<
							std::endl;
					}*/
					break;
				}

				case MessageType::PLAYER_INIT:
				{
					msg->base_message_ptr =
						SpawnNewMessage<PlayerInitMessage>(std::move(*ptr->base_message_ptr));
					break;
				}
				}
				msg->base_message_ptr->DecodeMessageBody(ptr->body_buffer);
				recv_msg_dispatcher_.Push(std::move(msg));
			}
		});

	game_server.Start();
	while (running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	recv_msg_dispatcher_.Stop();
	send_msg_dispatcher_.Stop();
	decode_thread.join();

	return 0;
}

void FooClient(asio::io_context& context)
{
	Buffer init_buffer;
	PlayerInitMessage init_message;
	init_message.role_id = 101010;
	init_message.EncodeMessage(init_buffer);

	Buffer buffer;
	ControlMessage message;
	message.player_id = 101010;
	message.tick = 1234;
	message.control_type = ControlMessage::ControlType::DOWN;
	message.EncodeMessage(buffer);

	asio::ip::tcp::socket socket(context);
	asio::ip::tcp::resolver resolver(context);
	auto endpoint = resolver.resolve("127.0.0.1", "40000");
	std::vector<GameConnectionPtr> game_conn_vec;
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		auto sock = asio::ip::tcp::socket(context);
		asio::connect(sock, endpoint);
		auto conn = std::make_shared<TcpConnection>(std::move(sock));
		conn->SetConnectionName("client conn: " + std::to_string(i));
		game_conn_vec.emplace_back(std::make_shared<GameConnection>(conn));

		game_conn_vec[i]->SetOnNewMsgWithBufferAndIdFunc(
			[](BaseMsgWithBufferAndIdPtr&& ptr)
			{
				std::cout << std::format("{}{}", ptr->base_message_ptr->DebugMessage(), CRLF);
			});

		game_conn_vec[i]->AsyncSendData(init_buffer.ReadBegin(), init_buffer.ReadableSize());
		game_conn_vec[i]->StartRecvData();
	}
	std::cout << "begin send " << std::endl;

	for (int i = 0; i < PACK_SUM; ++i)
	{
		game_conn_vec[i % THREAD_NUM]->AsyncSendData(buffer.ReadBegin(), buffer.ReadableSize());
	}
	context.run();
}

TEST(GameServer, RecvMsg)
{
	GameServer game_server("TestServer", 40000);
	bool running = true;

	std::thread t1(
		[&game_server, &running]()
		{ 
			FooMain(game_server, running);
		});
	std::this_thread::sleep_for(std::chrono::seconds(1));

	asio::io_context client_io;
	asio::executor_work_guard<asio::io_context::executor_type> client_guard{
		asio::make_work_guard(client_io) };
	std::thread t2(
		[&client_io]()
		{
			FooClient(client_io);
		});
	
	std::this_thread::sleep_for(std::chrono::seconds(2));

	client_guard.reset();
	client_io.stop();

	game_server.Stop();

	running = false;
	t1.join();
	t2.join();
}