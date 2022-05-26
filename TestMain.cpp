#include "GameServer/GameServerExample.h"
#include "GameClient/GameClient.h"
#include "Utils/TimeUtils.h"

void EncodeTest()
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
static int TEST_TIME = 5;
static int SUM_PACK = TEST_TIME * PACK_PER_PLAYER_PER_SEC * PLAYER_NUM;

static uint64_t sum_delay = 0;
static int count = 0;

void FooClient(GameClient& client)
{
	std::string address = "127.0.0.1";
	std::string port = "40000";
	std::vector<GameClientPtr> game_client_vec;

	PingMessagePtr ping_message_ptr = std::make_shared<PingMessage>();
	ping_message_ptr->role_id = 101010;

	client.SetMsgCallback(MessageType::PING,
			[](ROLE_ID role_id, BaseMessagePtr message_ptr)
			{
				PingMessagePtr msg = CastBaseMsgTo<PingMessage>(message_ptr);
				uint64_t now = NOW_MS;

				uint64_t delay = now - msg->timestamp;
				count++;
				sum_delay += delay;
			});

	for (int i = 0; i < PLAYER_NUM; ++i)
	{
		client.Connect(std::to_string(i), address, port);
	}
	std::cout << "begin send " << std::endl;

	for (int time = 0; time < TEST_TIME; ++time)
	{
		auto begin_time = std::chrono::system_clock::now();
		for (int pack_sub = 0; pack_sub < PACK_PER_PLAYER_PER_SEC; ++pack_sub)
		{
			auto frame_begin_time = NOW_MS;
			{
				for (int player_sub = 0; player_sub < PLAYER_NUM; ++player_sub)
				{
					ping_message_ptr->timestamp = NOW_MS;
					client.SendMsg(std::to_string(player_sub), ping_message_ptr);
				}
			}
			auto frame_end_time = NOW_MS;
			// sleep
			int sleep_ms = 1000 / PACK_PER_PLAYER_PER_SEC -
			               (frame_end_time - frame_begin_time);
			if (sleep_ms > 0)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms));
			}
			else
			{
				printf("Error\r\n");
			}
		}
		auto end_time = std::chrono::system_clock::now();
		printf("%lld ms\r\n",
				TO_MS(end_time - begin_time).count());
	}
}

void GameServerTest()
{
	GameServerExample server("TestServer", 40000);
	GameClient client("TestClient");
	std::thread t1(
		[&server]()
		{
			server.Start();
		});

    if (true)
    {
	    std::this_thread::sleep_for(std::chrono::seconds(1));
	    std::thread t2(
			    [&client]()
			    {
				    FooClient(client);
			    });

	    while (count != SUM_PACK)
	    {
		    std::this_thread::sleep_for(std::chrono::milliseconds(500));
			std::cout << count << std::endl;
	    }
	    std::cout << fmt::format("ping-{}: avg {}ms\r\n",
			    count, static_cast<float>(sum_delay) / count);

	    server.Stop();
	    client.Stop();
	    t2.join();
	}

	t1.join();
}

int main()
{
	EncodeTest();
	GameServerTest();

	return 0;
}