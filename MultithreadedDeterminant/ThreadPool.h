#pragma once

#include <vector>
#include <cassert>
#include <memory>
#include "BlockingQueue.h"

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
	BlockingQueue<std::unique_ptr<Task>> m_Tasks;
	std::vector<std::thread> m_Threads;
	bool m_PendingTermination;

	void RunThread();
};

