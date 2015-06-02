#include "ThreadPool.h"

Task::Task()
	: m_ShouldExit(false)
{
}

Task::Task(bool shouldExit)
	: m_ShouldExit(shouldExit)
{
}

bool Task::ShouldExit() const
{
	return m_ShouldExit;
}

void Task::Solve()
{
	std::cout << "From thread " << std::this_thread::get_id() << " - haai!" << std::endl;
}

ThreadPool::ThreadPool(unsigned threadCount)
	: m_Threads(threadCount)
{
	for (unsigned i = 0; i < threadCount; i++)
	{
		m_Threads.push_back(std::thread([this]() { this->RunThread(); }));
	}
}

ThreadPool::~ThreadPool()
{
	if (m_Threads.size() != 0)
	{
		FinishWork();
	}
}

void ThreadPool::FinishWork()
{
	for (int i = 0; i < m_Threads.size(); ++i)
	{
		m_Tasks.Enqueue(std::unique_ptr<Task>(new Task(true)));
	}
	for (auto& thread : m_Threads)
	{
		if (thread.joinable())
		{
			thread.join();
		}
	}
	m_Threads.clear();
}

void ThreadPool::AddTask(Task* task)
{
	m_Tasks.Enqueue(std::unique_ptr<Task>(task));
}

void ThreadPool::RunThread()
{
	for (;;)
	{
		std::unique_ptr<Task> task = std::move(m_Tasks.Dequeue());
		if (task->ShouldExit())
		{
			std::cout << "Thread Exiting" << std::endl;
			return;
		}
		task->Solve();
	}
}