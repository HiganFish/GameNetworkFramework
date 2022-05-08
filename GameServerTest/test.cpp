#include "pch.h"
#include <thread>
#include "ControlMessage.h"
#include "GameServer.h"
#include "ThreadSafeQueue.h"
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
	ptr->DecodeMessage(buffer);
	assert(buffer.ReadableSize() == 0);

	if (ptr->message_type == MessageType::CONTROL)
	{
		ControlMessage new_message(std::move(*ptr));
		std::cout << new_message.DebugMessage() << std::endl;
	}
}

int FooMain(GameServer& game_server, bool& running)
{
	// bool running = true;
	// GameServer game_server("TestServer", 40000);

	ThreadSafeQueue<BaseMsgWithRoleIdPtr> new_pack_queue;
	ThreadSafeQueue<BaseMsgWithRoleIdPtr> wait_for_send_queue;
	
	game_server.SetOnNewMsgWithIdFunc(
		[&new_pack_queue](BaseMsgWithRoleIdPtr&& ptr)
		{
			new_pack_queue.Push(std::move(ptr));
		});
	std::thread encode_thread(
		[&running, &wait_for_send_queue, &game_server]()
		{
			while (running)
			{
				BaseMsgWithRoleIdPtr ptr = wait_for_send_queue.Pop();
				TinyBuffer buffer;
				ptr->base_message_ptr->EncodeMessage(buffer);
				game_server.SendMessageById(ptr->role_id, buffer.ReadBegin(), buffer.ReadableSize());
			}
		});
	std::thread decode_thread(
		[&running, &new_pack_queue]()
		{
			while (running)
			{
				BaseMsgWithRoleIdPtr ptr = new_pack_queue.Pop();
				if (ptr->base_message_ptr->message_type == MessageType::CONTROL)
				{
					ControlMessagePtr message_ptr =
						std::make_shared<ControlMessage>(std::move(*ptr->base_message_ptr));
					// BaseMessagePtr message_ptr = SpawnNewMessage<ControlMessage>(*ptr->base_message_ptr);
					std::cout << std::format("{}{}", message_ptr->DebugMessage(), CRLF);
				}
			}
		});
	std::thread canncel_thread(
		[&running, &game_server]()
		{
			while (running)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(500));
			}
			game_server.Stop();
		});

	game_server.Start();

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
	asio::connect(socket, resolver.resolve("127.0.0.1", "40000"));

	for (int i = 0; i < 10; ++i)
	{
		socket.send(asio::buffer(buffer.ReadBegin(), buffer.ReadableSize()));
	}
	
	std::this_thread::sleep_for(std::chrono::seconds(5));

	running = false;
}