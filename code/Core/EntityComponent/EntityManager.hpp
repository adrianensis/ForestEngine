#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/Entity.hpp"

class EntityManager: public Singleton<EntityManager>
{
public:
    void init() {}
    void terminate() { mPool.terminate(); }

    template<class T> T_EXTENDS(T, Entity)
    TEntityPtr<T> requestEntity()
    {
        PoolElementPtr poolPtr = mPool.requestElement<T>();
        EntityPtr entityPtr = poolPtr;
        if(entityPtr.isValid())
        {
            T& entity = mPool.getElement<T>(poolPtr);
            entity.onRecycle(entityPtr.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }

        return entityPtr;
    }

    void removeEntity(EntityPtr& entityPtr)
    {
        mPool.removeElement(entityPtr);
        entityPtr.reset();
    }

private:
    Pool<Entity> mPool;

public:
    CRGET(Pool)
};
REGISTER_CLASS(EntityManager);