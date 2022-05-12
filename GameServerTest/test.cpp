#include "pch.h"
#include "GameServerExample.h"

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

void foo () {
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
	MKServer* mkserver;
	std::thread t1(
		[&mkserver]()
		{ 
			GameServerExample server("TestServer", 40000);
			mkserver = &server;
			server.Start();
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
	mkserver->Stop();

	t1.join();
	t2.join();
}