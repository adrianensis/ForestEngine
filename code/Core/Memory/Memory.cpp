#include "Core/Memory/Memory.hpp"
#include "Core/Log/Log.hpp"
#include "Core/Assert/Assert.hpp"

void Memory::init()
{
    MemoryTracking::init();
}

void Memory::terminate()
{
    MemoryTracking::terminate();
}
