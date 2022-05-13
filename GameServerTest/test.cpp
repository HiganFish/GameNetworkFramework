#include "pch.h"
#include "GameServerExample.h"

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

static int PACK_SUM = 0;
static int THREAD_NUM = 2000;

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

#define NOW_MS std::chrono::duration_cast<std::chrono::milliseconds>(	\
std::chrono::system_clock::now().time_since_epoch()).count();

void FooClient(asio::io_context& context)
{
	Buffer buffer;
	ControlMessage message;
	message.role_id = 101010;
	message.tick = 1234;
	message.control_type = ControlMessage::ControlType::DOWN;
	message.EncodeMessage(buffer);

	asio::ip::tcp::socket socket(context);
	asio::ip::tcp::resolver resolver(context);
	auto endpoint = resolver.resolve("127.0.0.1", "40000");
	std::vector<GameConnectionPtr> game_conn_vec;

	Buffer ping_buffer;
	PingMessage ping_message;
	ping_message.role_id = 101010;

	for (int i = 0; i < THREAD_NUM; ++i)
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
					static int count = 0;
					count++;
					std::cout << std::format("ping-{}: {}ms\r\n", count, now - msg->timestamp);
				}
			});

		game_conn_vec[i]->StartRecvData();

		ping_message.timestamp = NOW_MS;
		ping_buffer.Reset();
		ping_message.EncodeMessage(ping_buffer);
		game_conn_vec[i]->AsyncSendData(ping_buffer.ReadBegin(), ping_buffer.ReadableSize());
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
	
	std::this_thread::sleep_for(std::chrono::seconds(5));

	client_guard.reset();
	client_io.stop();
	mkserver->Stop();

	t1.join();
	t2.join();
}