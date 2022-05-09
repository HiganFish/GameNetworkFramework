#include "MsgThread.h"

MsgThread::MsgThread(bool& started, const DispathFunc& func) :
	started_(started),
	dispatch_func_(func),
	msg_queue_(),
	thread_([this]() {Running(); })
{

}

MsgThread::~MsgThread()
{
	Join();
}

void MsgThread::Running()
{
	while (started_)
	{
		BaseMsgWithRoleIdPtr msg_ptr = msg_queue_.Pop();
		if (dispatch_func_)
		{
			dispatch_func_(std::move(msg_ptr));
		}
	}
}

void MsgThread::Push(BaseMsgWithRoleIdPtr&& msg)
{
	msg_queue_.Push(std::move(msg));
}

void MsgThread::Join()
{
	assert(!started_);
	if (thread_.joinable())
	{
		thread_.join();
	}
	else
	{
		thread_.detach();
	}
	
}
