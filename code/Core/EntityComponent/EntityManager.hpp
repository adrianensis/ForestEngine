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
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        Slot slot = mPool.requestElement<T>();
        EntityPtr entityPtr(classId, slot);
        if(entityPtr.isValid())
        {
            T& entity = mPool.getElement<T>(slot);
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
        mPool.removeElement(entityPtr.mClassId, entityPtr.mSlot);
        entityPtr.reset();
    }

private:
    Pool<Entity> mPool;

public:
    CRGET(Pool)
};
REGISTER_CLASS(EntityManager);