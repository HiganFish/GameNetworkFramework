#pragma once

#include "GameServerExample.h"

int main()
{
	GameServerExample server("TestServer", 40000);

	server.Start();

	server.Stop();
	return 0;
}

