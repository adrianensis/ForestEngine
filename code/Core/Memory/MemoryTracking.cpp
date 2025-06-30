#include "Core/Memory/MemoryTracking.hpp"
#include "Core/Log/Log.hpp"

NS_BEGIN(Core)
void MemoryTracking::init()
{
}

void MemoryTracking::log()
{
#ifdef ENGINE_BUILD_DEBUG
	LOG("-------- MEM SUMMARY --------")
	FOR_MAP(it, MemoryTracking::smAllocationsMap)
	{
		LOG(it->first.get() + ": " + std::to_string(it->second.mCurrentAllocations) + " (max: " + std::to_string(it->second.mMaxAllocations) + ")");
	}
	LOG("-----------------------------")
#endif

}
void MemoryTracking::terminate()
{
	log();
}

NS_END