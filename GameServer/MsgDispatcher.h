#pragma once
#include <array>
#include "MsgThread.h"

class MsgDispatcher
{
public:
	using MsgCallback = std::function<void(ROLE_ID, BaseMessagePtr)>;

	MsgDispatcher(uint32_t thread_num);
	MsgDispatcher(const MsgDispatcher&) = delete;
	MsgDispatcher& operator=(const MsgDispatcher&) = delete;

	void Push(BaseMsgWithRoleIdPtr&& msg);

	bool Start();

	void Stop();

	void Dispatch(BaseMsgWithRoleIdPtr msg_ptr);

	void SetMsgCallback(MessageType msg_type, const MsgCallback& callback);

private:
	bool started_;
	uint32_t thread_num_;
	std::vector<std::shared_ptr<MsgThread>> msg_threads_;

	std::vector<MsgCallback> msg_callbacks_;
};

