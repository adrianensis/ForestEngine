#include "Core/Memory/Memory.hpp"
#include "Core/Log/Log.hpp"


void Memory::init()
{
}

void Memory::terminate()
{
#ifdef ENGINE_DEBUG
	LOG("-------- MEM SUMMARY --------")
	FOR_MAP(it, Memory::mAllocationsCounter)
	{
		std::cout << it->first << ": " << it->second.mCurrentAllocations << " (max: " << it->second.mMaxAllocations << ")" << std::endl;
	}
	LOG("-----------------------------")
#endif

}
