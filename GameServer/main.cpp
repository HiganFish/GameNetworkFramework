#include "GameServer.h"

int main()
{
	GameServer game_server("TestServer", 40000);

	game_server.Start();

	return 0;
}