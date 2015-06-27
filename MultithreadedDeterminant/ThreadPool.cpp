#include "ThreadPool.h"
#include <gmpxx.h>
#include <map>
#include "Logger.h"

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

#define MB << 20

static char* g_Buffer;
static int g_ThreadCount = 0;
static size_t* g_ThreadMapping;
static THREAD_LOCAL int tl_ThreadIndex;
static THREAD_LOCAL size_t tl_BufferUsedSize;
static const unsigned HEAP_SIZE = 2000 MB;
static unsigned HEAP_SIZE_PER_THREAD = 0;

// Used to delay the start of all threads
std::mutex g_BeginExecutionMutex;


int GetCurrentThreadIndex()
{
	auto hash = std::this_thread::get_id().hash();
	for (size_t i = 0; i < g_ThreadCount; i++)
	{
		if (g_ThreadMapping[i] == hash)
		{
			return i;
		}
	}
	Logger::Instance().Error("No thread with hash: " + std::to_string(hash));
	return -1;
}

void* customMalloc(size_t size)
{
	auto id = tl_ThreadIndex;
	auto ptr = g_Buffer + HEAP_SIZE_PER_THREAD * id + tl_BufferUsedSize;
	tl_BufferUsedSize += size;
	return ptr;
}

void customFree(void*, size_t size)
{
}

void* customRealloc(void* ptr, size_t oldSize, size_t newSize)
{
	if (newSize < oldSize)
		return ptr;

	auto newPos = customMalloc(newSize);
	std::memcpy(newPos, ptr, oldSize);
	return newPos;
}

// Frees all memory of the current thread allocated after the allocation
// of the given ptr
void customFreePastPtr(void* ptr)
{
	auto id = tl_ThreadIndex;
	auto memoryBlockStart = g_Buffer + HEAP_SIZE_PER_THREAD * id;
	if (ptr < memoryBlockStart || ptr > memoryBlockStart + HEAP_SIZE_PER_THREAD)
	{
		Logger::Instance().Error("Attempted to free memory of another thread!");
	}
	auto memoryToBeFreed = static_cast<char*>(ptr) - memoryBlockStart;
	Logger::Instance().Info("Freeing " + std::to_string((tl_BufferUsedSize - memoryToBeFreed) >> 20) + " MB on thread " + std::to_string(id) + " / " + std::to_string(std::this_thread::get_id().hash()));
	tl_BufferUsedSize = static_cast<char*>(ptr)-memoryBlockStart;
}

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
}

ThreadPool::ThreadPool(unsigned threadCount)
	: m_ThreadCount(threadCount)
	, m_Threads(threadCount)
{
	// Add 1 to make place for the main thread
	auto actualThreads = threadCount + 1;
	HEAP_SIZE_PER_THREAD = HEAP_SIZE / actualThreads;
	g_Buffer = new char[HEAP_SIZE];
	g_ThreadMapping = new size_t[actualThreads];
	g_ThreadMapping[0] = tl_ThreadIndex = 0;
	g_ThreadCount = actualThreads;

	mp_set_memory_functions(&customMalloc,  &customRealloc, &customFree);

	Logger::Instance().Info("Thread pool initialized!");
	Logger::Instance().Info("Heap size in MB: " + std::to_string(HEAP_SIZE >> 20));
	Logger::Instance().Info("Heap size per thread: " + std::to_string(HEAP_SIZE_PER_THREAD >> 20));
	Logger::Instance().Info("GMP Version: " + std::string(gmp_version));
}

ThreadPool::~ThreadPool()
{
	if (m_Threads.size() != 0)
	{
		FinishWork();
	}
	delete[] g_Buffer;
	delete[] g_ThreadMapping;
}

void ThreadPool::FinishWork()
{
	// Add exit tasks
	for (unsigned i = 0; i < m_ThreadCount; i++)
	{
		AddTask(new Task(true));
	}

	{
		// Lock the mutex so that threads will not start until their id is properly set
		std::lock_guard<std::mutex> lock(g_BeginExecutionMutex);
		for (unsigned i = 0; i < m_ThreadCount; i++)
		{
			m_Threads.push_back(std::thread([this]() { this->RunThread(); }));
			g_ThreadMapping[i + 1] = m_Threads.back().get_id().hash();
		}
	}

	for (size_t i = 0; i < g_ThreadCount; i++)
	{
		Logger::Instance().Info("Thread " + std::to_string(i) + " has id " + std::to_string(g_ThreadMapping[i]));
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
	{
		// Wait until the id mapping is complete
		std::lock_guard<std::mutex> lock(g_BeginExecutionMutex);
	}
	tl_ThreadIndex = GetCurrentThreadIndex();

	Logger::Instance().Info("Thread went to work: " + std::to_string(std::this_thread::get_id().hash()));
	for (;;)
	{
		std::unique_ptr<Task> task = std::move(m_Tasks.Dequeue());
		void* memoryMarker = customMalloc(1);
		task->Solve();
		customFreePastPtr(memoryMarker);
		if (task->ShouldExit())
		{
			break;
		}
	}
	Logger::Instance().Info("Thread exiting: " + std::to_string(std::this_thread::get_id().hash()));
}