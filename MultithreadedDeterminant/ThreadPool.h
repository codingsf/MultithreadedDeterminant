#pragma once

#include <vector>
#include <cassert>
#include <memory>
#include "ThreadSafeQueue.h"

#include <iostream>


class Task
{
public:
	Task();
	Task(bool shouldExit);
	bool ShouldExit() const;
	virtual void Solve();
private:
	bool m_ShouldExit;
};

class ThreadPool
{
public:
	ThreadPool(unsigned threadCount);
	~ThreadPool();
	void AddTask(Task* task);
	void FinishWork();


private:
	ThreadSafeQueue<std::unique_ptr<Task>> m_Tasks;
	std::vector<std::thread> m_Threads;
	unsigned m_ThreadCount;
	bool m_PendingTermination;

	void RunThread();
};

