#pragma once

#include "Core/Core.hpp"
#include "Core/Memory/Pool.hpp"

NS_BEGIN(EC)

class Entity
{
public:
    Entity();
    virtual void onECComponentRecycle(Core::PoolElementPtr poolElementPtr);
    
protected:
    Core::PoolElementPtr mPoolElementPtr;

public:

    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    
	CRGET(PoolElementPtr)
};
REGISTER_CLASS(Entity);

NS_END