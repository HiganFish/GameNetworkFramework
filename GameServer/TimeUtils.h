#pragma once

#include <chrono>

#define NOW_MS std::chrono::duration_cast<std::chrono::milliseconds>(	\
std::chrono::system_clock::now().time_since_epoch()).count()

#define TO_US(tm) std::chrono::duration_cast<std::chrono::microseconds>(tm)