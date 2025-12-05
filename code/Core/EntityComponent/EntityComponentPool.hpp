#pragma once

#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/Memory/Pool.hpp"

NS_BEGIN(EC)

class EntityComponentPool
{
public:
    void init(Core::u32 size)
    {
        mEntitiesPool.init(size);
        mComponentsPool.init(size);
    }
    void terminate()
    { 
        mEntitiesPool.terminate();
        mComponentsPool.terminate();
    }

private:
    Core::Pool<Entity> mEntitiesPool;
    Core::Pool<Component> mComponentsPool;
public:
    RGET(EntitiesPool)
    CRGET(EntitiesPool)
    RGET(ComponentsPool)
    CRGET(ComponentsPool)
};
NS_END