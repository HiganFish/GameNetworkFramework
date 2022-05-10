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

static int PACK_SUM = 100;
static int THREAD_NUM = 10;

int FooMain(GameServer& game_server, bool& running)
{
	// bool running = true;
	// GameServer game_server("TestServer", 40000);

	ThreadSafeQueue<BaseMsgWithBufferAndIdPtr> new_pack_queue;
	ThreadSafeQueue<BaseMsgWithRoleIdPtr> wait_for_send_queue;
	MsgDispatcher msg_dispatcher_(1);
	msg_dispatcher_.Start();

	game_server.SetOnNewMsgWithBufferAndIdFunc(
		[&new_pack_queue](BaseMsgWithBufferAndIdPtr&& ptr)
		{
			new_pack_queue.Push(std::move(ptr));
		});
	std::thread encode_thread(
		[&running, &wait_for_send_queue, &game_server]()
		{
			while (running)
			{
				BaseMsgWithRoleIdPtr ptr;
				bool has = wait_for_send_queue.TryPop(ptr);
				if (has)
				{
					TinyBuffer buffer;
					ptr->base_message_ptr->EncodeMessage(buffer);
					game_server.SendMessageById(ptr->role_id, buffer.ReadBegin(), buffer.ReadableSize());
				}
			}
		});
	std::thread decode_thread(
		[&running, &new_pack_queue, &msg_dispatcher_]()
		{
			while (running)
			{
				BaseMsgWithBufferAndIdPtr ptr;
				bool has = new_pack_queue.TryPop(ptr);
				if (!has)
				{
					continue;
				}
				if (ptr->base_message_ptr->message_type == MessageType::CONTROL)
				{
					auto msg = std::make_shared<BaseMsgWithRoleId>();
					msg->role_id = ptr->role_id;
					msg->base_message_ptr =
						std::make_shared<ControlMessage>(std::move(*ptr->base_message_ptr));
					msg->base_message_ptr->DecodeMessageBody(ptr->body_buffer);

					msg_dispatcher_.Push(std::move(msg));


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
				}
			}
		});

	game_server.Start();
	while (running)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	msg_dispatcher_.Stop();
	encode_thread.join();
	decode_thread.join();

	return 0;
}

TEST(GameServer, RecvMsg)
{
	Buffer buffer;
	ControlMessage message;
	message.player_id = 101010;
	message.tick = 1234;
	message.control_type = ControlMessage::ControlType::DOWN;
	message.EncodeMessage(buffer);

	GameServer game_server("TestServer", 40000);
	bool running = true;

	std::thread t1(
		[&game_server, &running]()
		{ 
			FooMain(game_server, running);
		});
	std::this_thread::sleep_for(std::chrono::seconds(1));

	asio::io_context context;
	asio::ip::tcp::socket socket(context);
	asio::ip::tcp::resolver resolver(context);
	auto endpoint = resolver.resolve("127.0.0.1", "40000");
	std::vector< asio::ip::tcp::socket> socket_vec;
	for (int i = 0; i < THREAD_NUM; ++i)
	{
		socket_vec.emplace_back(context);
		asio::connect(socket_vec[i], endpoint);
	}
	std::cout << "begin send " << std::endl;

	for (int i = 0; i < PACK_SUM; ++i)
	{
		asio::async_write(socket_vec[i % THREAD_NUM], asio::buffer(buffer.ReadBegin(), buffer.ReadableSize()),
			[](const asio::error_code& ec, size_t length) {
				if (ec)
				{
					std::cout << ec.message() << std::endl;
				}
			});
	}
	
	std::this_thread::sleep_for(std::chrono::seconds(2));
	game_server.Stop();
	running = false;
	t1.join();
}