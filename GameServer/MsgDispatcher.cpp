#include <iostream>
#include "MsgDispatcher.h"

void DefaultMsgCallback(ROLE_ID role_id, BaseMessagePtr msg_ptr)
{
	std::cout << std::format("default msg callback {}-{}{}",
		role_id, msg_ptr->DebugMessage(), CRLF);
}


MsgDispatcher::MsgDispatcher(uint32_t thread_num) :
	started_(false),
	thread_num_(thread_num),
	default_message_callback_(DefaultMsgCallback),
	msg_threads_(),
	msg_callbacks_(
		std::to_underlying(MessageType::TYPE_MAX), default_message_callback_)
{
}

void MsgDispatcher::Push(BaseMsgWithRoleIdPtr&& msg)
{
	if (!msg || !started_)
	{
		return;
	}
	msg_threads_[msg->role_id % thread_num_]->Push(std::move(msg));
}

bool MsgDispatcher::Start()
{
	if (started_)
	{
		return false;
	}
	started_ = true;
	// msg_threads_如果直接保存 MsgThread而不是其指针
	// 由于MsgThread禁止拷贝构造 会导致编译无法通过
	// https://stackoverflow.com/questions/27470827/why-does-stdvectorreserve-call-the-copy-constructor
	msg_threads_.reserve(thread_num_);
	for (int i = 0; i < thread_num_; ++i)
	{
		auto msg_thread_ptr = std::make_shared<MsgThread>(started_, [this](auto&& PH1) {Dispatch(PH1); });
		msg_threads_.push_back(msg_thread_ptr);
	}
}

void MsgDispatcher::Stop()
{
	started_ = false;
	for (auto& thread : msg_threads_)
	{
		thread->Join();
	}
}

void MsgDispatcher::Dispatch(BaseMsgWithRoleIdPtr msg_ptr)
{
	auto role_id = msg_ptr->role_id;
	auto base_msg_ptr = msg_ptr->base_message_ptr;
	auto func = msg_callbacks_[std::to_underlying(base_msg_ptr->message_type)];
	func(role_id, base_msg_ptr);
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