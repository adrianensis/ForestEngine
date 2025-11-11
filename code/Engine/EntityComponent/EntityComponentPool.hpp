#pragma once

#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Core/Memory/Pool.hpp"

NS_BEGIN(EC)

class EntityComponentPool
{
public:
    void init()
    {
        mEntitiesPool.init(100000);
        mComponentsPool.init(100000);
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