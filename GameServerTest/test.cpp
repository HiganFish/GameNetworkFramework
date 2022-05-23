#include "pch.h"
#include "GameServerExample.h"
#include "TimeUtils.h"

TEST(Message, MessageCodec)
{
	Buffer buffer;
	
	ControlMessage message;
	message.role_id = 101010;
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

static int PLAYER_NUM = 100;
static int PACK_PER_PLAYER_PER_SEC = 20;
static int TEST_TIME = 10;
static int SUM_PACK = TEST_TIME * PACK_PER_PLAYER_PER_SEC * PLAYER_NUM;

static uint64_t sum_delay = 0;
static int count = 0;

void FooClient(asio::io_context& context)
{
	asio::ip::tcp::socket socket(context);
	asio::ip::tcp::resolver resolver(context);
	auto endpoint = resolver.resolve("127.0.0.1", "40000");
	// auto endpoint = resolver.resolve("192.168.50.200", "6666");
	std::vector<GameConnectionPtr> game_conn_vec;

	Buffer ping_buffer;
	PingMessage ping_message;
	ping_message.role_id = 101010;

	for (int i = 0; i < PLAYER_NUM; ++i)
	{
		auto sock = asio::ip::tcp::socket(context);
		asio::connect(sock, endpoint);
		auto conn = std::make_shared<TcpConnection>(std::move(sock));
		conn->SetConnectionName("client conn: " + std::to_string(i));
		game_conn_vec.emplace_back(std::make_shared<GameConnection>(conn));

		game_conn_vec[i]->SetOnNewMsgWithBufferFunc(
			[](BaseMsgWithBufferPtr&& ptr)
			{
				// std::cout << std::format("{}\r\n", ptr->base_message_ptr->DebugMessage());
				if (ptr->base_message_ptr->message_type == MessageType::PING)
				{
					PingMessagePtr msg = std::dynamic_pointer_cast<PingMessage>(TransmitMessage(ptr));

					uint64_t now = NOW_MS;

					uint64_t delay = now - msg->timestamp;
					count++;

					// std::cout << std::format("{}-{}\r\n", count, delay);
					sum_delay += delay;
				}
			});

		game_conn_vec[i]->StartRecvData();
	}
	std::cout << "begin send " << std::endl;

	for (int time = 0; time < TEST_TIME; ++time)
	{
		auto begin_time = std::chrono::system_clock::now();
		for (int pack_sub = 0; pack_sub < PACK_PER_PLAYER_PER_SEC; ++pack_sub)
		{
			auto frame_begin_time = std::chrono::system_clock::now();
			{
				for (int player_sub = 0; player_sub < PLAYER_NUM; ++player_sub)
				{
					ping_message.timestamp = NOW_MS;
					ping_buffer.Reset();
					ping_message.EncodeMessage(ping_buffer);

					game_conn_vec[player_sub]->AsyncSendData(ping_buffer.ReadBegin(), ping_buffer.ReadableSize());
				}
			}
			// sleep
		}
		auto end_time = std::chrono::system_clock::now();
		printf("%lld\r\n", 
			TO_US(end_time - begin_time).count());
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
	
	while (count != SUM_PACK)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
	std::cout << std::format("ping-{}: avg {}ms\r\n",
		count, sum_delay / count);
	client_guard.reset();
	client_io.stop();
	mkserver->Stop();

	t1.join();
	t2.join();
}