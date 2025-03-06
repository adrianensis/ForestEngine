#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/EntityPtr.hpp"

class EntityManager: public Singleton<EntityManager>
{
public:
    void init() {}
    void terminate() { mPoolsManager.terminate(); }

    template<class T> T_EXTENDS(T, Entity)
    TEntityPtr<T> requestEntity()
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        Slot slot = mPoolsManager.requestElement<T>();
        EntityPtr entityPtr(classId, slot, this);
        if(entityPtr.isValid())
        {
            T& entity = mPoolsManager.getElement<T>(slot);
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
        mPoolsManager.removeElement(entityPtr.mClassId, entityPtr.mSlot);
        entityPtr.reset();
    }

private:
    PoolsManager<Entity> mPoolsManager;

public:
    CRGET(PoolsManager)
};
REGISTER_CLASS(EntityManager);