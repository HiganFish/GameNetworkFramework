#include <iostream>
#include "MsgDispatcher.h"

void DefaultMsgCallback(ROLE_ID role_id, BaseMessagePtr msg_ptr)
{
	std::cout << std::format("default msg callback {}-{}\r\n",
		role_id, msg_ptr->DebugMessage());
}


MsgDispatcher::MsgDispatcher(uint32_t thread_num) :
	started_(false),
	thread_num_(thread_num),
	default_message_callback_(DefaultMsgCallback),
	msg_threads_(),
	msg_callbacks_(
		std::to_underlying(MessageType::TYPE_MAX), nullptr)
{
}

void MsgDispatcher::Push(ROLE_ID dispatch_id, const BaseMessagePtr& msg)
{
	if (!msg || !started_)
	{
		return;
	}
	msg_threads_[dispatch_id % thread_num_]->
		Push({ dispatch_id, msg});
}

void MsgDispatcher::Push(const std::vector<ROLE_ID>& dispatch_ids,
	const BaseMessagePtr& msg)
{
	for (auto dispatch_id : dispatch_ids)
	{
		Push(dispatch_id, msg);
	}
}

bool MsgDispatcher::Start()
{
	if (started_)
	{
		return false;
	}
	started_ = true;
	// because copy constructor deleted, resize vec will be error
	// https://stackoverflow.com/questions/27470827/why-does-stdvectorreserve-call-the-copy-constructor
	msg_threads_.reserve(thread_num_);
	for (uint32_t i = 0; i < thread_num_; ++i)
	{
		auto msg_thread_ptr = 
			std::make_shared<MsgThread<MsgForDispatch>>(started_, [this](auto&& PH1) {Dispatch(PH1); });
		msg_threads_.push_back(msg_thread_ptr);
	}
	return true;
}

void MsgDispatcher::Stop()
{
	started_ = false;
	for (auto& thread : msg_threads_)
	{
		thread->Join();
	}
}

void MsgDispatcher::Dispatch(MsgForDispatch& msg)
{
	auto msg_ptr = msg.base_message_ptr;
	auto role_id = msg_ptr->role_id;
	auto func = msg_callbacks_[std::to_underlying(msg_ptr->message_type)];
	if (!func)
	{
		func = default_message_callback_;
	}
	func(role_id, std::move(msg_ptr));
}

void MsgDispatcher::SetMsgCallback(MessageType msg_type, const MsgCallback& callback)
{
	assert(msg_type > MessageType::TYTE_MIN && 
					msg_type < MessageType::TYPE_MAX);
	
	msg_callbacks_[std::to_underlying(msg_type)] = callback;
}
void MsgDispatcher::SetDefaultMsgCallback(const MsgCallback& callback)
{
	default_message_callback_ = callback;
}