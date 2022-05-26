//
// Created by rjd
//

#include <GameServer/GameServerExample.h>

int main()
{
	GameServerExample server("TestServer", 4000);

	server.Start();
}