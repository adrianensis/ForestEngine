#include "Core/Memory/Memory.hpp"
#include "Core/Log/Log.hpp"

void Memory::init()
{
}

void Memory::terminate()
{
#ifdef ENGINE_DEBUG
	LOG("-------- MEM SUMMARY --------")
	FOR_MAP(it, Memory::smAllocationsCounter)
	{
		LOG(std::string(it->first) + ": " + std::to_string(it->second.mCurrentAllocations) + " (max: " + std::to_string(it->second.mMaxAllocations) + ")");
	}
	LOG("-----------------------------")
#endif

}
