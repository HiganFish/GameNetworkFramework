#pragma once

#include <thread>
#include <functional>
#include "Utils/ThreadSafeQueue.h"
#include "Message/Messages.h"

template <typename MSG_PTR>
class MsgThread
{
public:
	using DispathFunc = std::function<void(MSG_PTR)>;
	MsgThread(bool& started, const DispathFunc& func) :
		started_(started),
		dispatch_func_(func),
		msg_queue_(),
		thread_([this]() {Running(); })
	{

	}

	~MsgThread()
	{
		if (started_)
		{
			Join();
		}
	}
	MsgThread(const MsgThread&) = delete;
	MsgThread& operator=(const MsgThread&) = delete;

	void Running()
	{
		while (started_)
		{
			MSG_PTR msg_ptr;
			bool has_pack = msg_queue_.TryPop(msg_ptr);
			if (has_pack && dispatch_func_)
			{
				// printf("%p-%zu\r\n", this, msg_queue_.Size());
				dispatch_func_(msg_ptr);
			}
		}
	}

	void Push(MSG_PTR&& msg)
	{
		msg_queue_.Push(std::forward<MSG_PTR>(msg));
	}

	void Join()
	{
		assert(!started_);
		thread_.join();
	}


private:
	bool& started_;
	DispathFunc dispatch_func_;
	ThreadSafeQueue<MSG_PTR> msg_queue_;
	std::thread thread_;
};