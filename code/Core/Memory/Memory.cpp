#include "Core/Memory/Memory.hpp"

NS_BEGIN(Core)
void Memory::init()
{
    MemoryTracking::init();
}

void Memory::terminate()
{
    MemoryTracking::terminate();
}

NS_END