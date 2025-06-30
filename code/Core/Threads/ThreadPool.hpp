#pragma once

#include "Core/Threads/Thread.hpp"


NS_BEGIN(Core)
class ThreadPool
{
public:
	std::vector<OwnerPtr<Thread>> threads;

	// Sets the number of threads to be allocated in this pool
	void setThreadCount(uint32_t count)
	{
		threads.clear();
		for (uint32_t i = 0; i < count; i++)
		{
			threads.push_back(OwnerPtr<Thread>::newObject());
		}
	}

	// Wait until all threads have finished their work items
	void wait()
	{
		for (auto &thread : threads)
		{
			thread->wait();
		}
	}
};

NS_END