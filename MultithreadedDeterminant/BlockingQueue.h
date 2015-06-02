#pragma once
#include <queue>
#include <thread>
#include <condition_variable>
#include <mutex>

template<class T>
class BlockingQueue
{
private:
	std::queue<T> m_Queue;
	std::condition_variable m_Condition;
	std::mutex m_Mutex;
public:

	void Enqueue(const T& value)
	{
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Queue.push(value);
		}
		m_Condition.notify_one();
	}

	void Enqueue(T&& value)
	{
		{
			std::unique_lock<std::mutex> lock(m_Mutex);
			m_Queue.push(std::forward<T>(value));
		}
		m_Condition.notify_one();
	}

	T Dequeue()
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		auto predicate = [this]() -> bool { return !m_Queue.empty(); };
		m_Condition.wait(lock, predicate);
		T first(std::move(m_Queue.front()));
		m_Queue.pop();
		return first;
	}

	std::size_t Size()
	{
		return m_Queue.size();
	}
};