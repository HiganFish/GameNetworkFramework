#pragma once
#include <array>
#include <functional>

#include "MsgThread.h"

struct MsgForDispatch
{
	ROLE_ID dispatch_id;
	BaseMessagePtr base_message_ptr;
};
using MsgForDispatchPtr = std::shared_ptr<MsgForDispatch>;

class MsgDispatcher
{
public:
	using MsgCallback = std::function<void(ROLE_ID, const BaseMessagePtr&)>;

	MsgDispatcher(uint32_t thread_num);
	~MsgDispatcher();
	MsgDispatcher(const MsgDispatcher&) = delete;
	MsgDispatcher& operator=(const MsgDispatcher&) = delete;

	void Push(ROLE_ID dispatch_id, const BaseMessagePtr& msg);
	void Push(const std::vector<ROLE_ID>& dispatch_ids,
		const BaseMessagePtr& msg);

	bool Start();

	void Stop();

	void Dispatch(MsgForDispatch& msg_ptr);

	void SetMsgCallback(MessageType msg_type, const MsgCallback& callback);

	void SetDefaultMsgCallback(const MsgCallback& callback);

private:
	bool started_;
	uint32_t thread_num_;
	MsgCallback default_message_callback_;
	std::vector<std::shared_ptr<MsgThread<MsgForDispatch>>> msg_threads_;

	std::vector<MsgCallback> msg_callbacks_;
};

