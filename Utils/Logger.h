//
// Created by rjd
//

#ifndef GAMEPROJECT_LOGGER_H
#define GAMEPROJECT_LOGGER_H

#include <string>
#include <memory>
#include <vector>
#include "FormatUtils.h"

template<class ...Args>
std::string string_format(const char* format, Args&& ... args)
{
	size_t size =
			std::snprintf(nullptr, 0,
					format, std::forward<Args>(args)...);
	std::string rst;
	rst.resize(size);
	std::snprintf(&rst[0], size + 1,
			format, std::forward<Args>(args)...);
	return rst;
}

class Logger
{
public:

	enum LOG_LEVEL
	{
		INFO,
		MAX_LEVEL
	};

	explicit Logger(LOG_LEVEL level, const char* file_name, int line_num);
	~Logger();

	Logger& operator<<(int32_t number);
	Logger& operator<<(uint32_t number);
	Logger& operator<<(int64_t number);
	Logger& operator<<(uint64_t number);
	Logger& operator<<(const std::string& str);

	template<class T>
	Logger& operator<<(const std::vector<T>& vec)
	{
		return *this << VectorToString(vec);
	}

private:

	Logger::LOG_LEVEL level_;
	const char* file_name_;
	int line_num_;
	std::string buffer_;
};
#ifdef _WIN32
#define CRLF "\r\n"
#define FILE_SEP '\\'
#else
#define CRLF "\n"
#define FILE_SEP '/'
#endif
#define FILENAME (strrchr(__FILE__, FILE_SEP) ? strrchr(__FILE__, FILE_SEP) + 1 : __FILE__)
#define LOG_INFO Logger(Logger::LOG_LEVEL::INFO, FILENAME, __LINE__)

#endif //GAMEPROJECT_LOGGER_H
