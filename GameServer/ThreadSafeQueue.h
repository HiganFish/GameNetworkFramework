#pragma once

#include <queue>
#include <mutex>
#include <chrono>

template <typename T>
class ThreadSafeQueue
{
public:
	ThreadSafeQueue() = default;
	~ThreadSafeQueue() = default;

	ThreadSafeQueue(const ThreadSafeQueue&) = delete;
	ThreadSafeQueue& operator=(const ThreadSafeQueue&) = delete;

	void Push(T&& t)
	{
		std::lock_guard guard(queue_mutex_);
		deque_.push_back(std::forward<T>(t));
		cond_.notify_one();
	}

	T Pop()
	{
		std::unique_lock lock(queue_mutex_);
		while (deque_.empty())
		{
			cond_.wait(lock);
		}
		T t = deque_.front();
		deque_.pop_front();
		return t;
	}

	bool TryPop(T& t, const std::chrono::milliseconds& timeout = std::chrono::milliseconds(10))
	{
		std::unique_lock lock(queue_mutex_);
		while (deque_.empty())
		{
			if (cond_.wait_for(lock, timeout) == std::cv_status::timeout)
			{
				return false;
			}
		}
		t = std::move(deque_.front());
		deque_.pop_front();
		return true;
	}

	size_t Size() const
	{
		return deque_.size();
	}
private:
	std::mutex queue_mutex_;
	std::condition_variable  cond_;
	std::deque<T> deque_;
};
