#pragma once

#include <thread>
#include <functional>
#include "ThreadSafeQueue.h"
#include "Messages.h"

class MsgThread
{
public:
	using DispathFunc = std::function<void(BaseMsgWithRoleIdPtr)>;
	MsgThread(bool& started, const DispathFunc&);
	~MsgThread();
	MsgThread(const MsgThread&) = delete;
	MsgThread& operator=(const MsgThread&) = delete;

	void Running();

	void Push(BaseMsgWithRoleIdPtr&& msg);

	void Join();

private:
	bool& started_;
	DispathFunc dispatch_func_;
	ThreadSafeQueue<BaseMsgWithRoleIdPtr> msg_queue_;
	std::thread thread_;
};