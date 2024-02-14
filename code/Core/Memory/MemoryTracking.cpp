#include "Core/Memory/MemoryTracking.hpp"
#include "Core/Log/Log.hpp"

void MemoryTracking::init()
{
}

void MemoryTracking::terminate()
{
#ifdef ENGINE_DEBUG
	LOG("-------- MEM SUMMARY --------")
	FOR_MAP(it, MemoryTracking::smAllocationsMap)
	{
		LOG(std::string(it->first) + ": " + std::to_string(it->second.mCurrentAllocations) + " (max: " + std::to_string(it->second.mMaxAllocations) + ")");
	}
	LOG("-----------------------------")
#endif

}
