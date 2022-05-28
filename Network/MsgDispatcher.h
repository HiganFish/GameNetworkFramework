#pragma once
#include <array>
#include <functional>
#include <future>

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

	MsgDispatcher(uint32_t thread_num, bool enable_sync = false);
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

	/**
	 * create message unique id by role_id for send sync message
	 * @param role_id
	 * @return the unique id corresponding future
	 */
	std::future<BaseMessagePtr> SetSyncMessageId(ROLE_ID role_id, const BaseMessagePtr& message);

private:
	bool started_;
	uint32_t thread_num_;
	MsgCallback default_message_callback_;
	std::vector<std::shared_ptr<MsgThread<MsgForDispatch>>> msg_threads_;

	std::vector<MsgCallback> msg_callbacks_;

	bool enable_sync_;
	const uint32_t ASYNC_MESSAGE_UNIQUE_ID = 0;
	std::unordered_map<ROLE_ID, std::atomic_uint32_t> message_id_map_;
	std::unordered_map<uint32_t, std::promise<BaseMessagePtr>> message_promise_;
	std::mutex message_promise_mutex_;
};

