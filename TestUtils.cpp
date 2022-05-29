//
// Created by rjd
//

#include "Utils/Logger.h"

int main()
{
	LOG_INFO << 123 << 456 << "123";
	LOG_INFO << string_format("%d, %d", 1, 1);
	return 0;
}