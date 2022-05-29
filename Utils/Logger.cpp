//
// Created by rjd
//
#include <chrono>
#include <sstream>
#include <iomanip>
#include "Logger.h"

const char* LogLevelName[static_cast<int>(Logger::MAX_LEVEL)] =
		{
		"INFO "
		};

Logger::Logger(Logger::LOG_LEVEL level, const char* file_name, int line_num):
	level_(level),
	file_name_(file_name),
	line_num_(line_num),
	buffer_()
{
}

static std::string GetCurrentTime()
{
	auto now = std::chrono::system_clock::now();
	auto in_time_t = std::chrono::system_clock::to_time_t(now);

	std::stringstream ss;
	ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
	return ss.str();
}
Logger::~Logger()
{
	std::chrono::system_clock::now();
	printf("%s %s - %s - %s:%d%s", GetCurrentTime().c_str(),
			LogLevelName[static_cast<int>(level_)], buffer_.c_str(),
			file_name_, line_num_, CRLF);
}

Logger& Logger::operator<<(int32_t number)
{
	return *this << std::to_string(number);
}

Logger& Logger::operator<<(uint32_t number)
{
	return *this << std::to_string(number);
}

Logger& Logger::operator<<(int64_t number)
{
	return *this << std::to_string(number);
}

Logger& Logger::operator<<(uint64_t number)
{
	return *this << std::to_string(number);
}

Logger& Logger::operator<<(const std::string& str)
{
	buffer_ += str;
	return *this;
}
