#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/EntityHandler.hpp"

class EntityManager: public Singleton<EntityManager>
{
public:
    void init() {}
    void terminate() { mPoolsManager.terminate(); }

    template<class T> T_EXTENDS(T, Entity)
    TEntityHandler<T> requestEntity()
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        Slot slot = mPoolsManager.requestElement<T>();
        EntityHandler entityHandler(classId, slot, this);
        if(entityHandler.isValid())
        {
            Entity& entity = mPoolsManager.getElementBase(classId, slot);
            T* entityT = static_cast<T*>(&entity);
            *entityT = T();
            entityT->onRecycle(entityHandler.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }

        return entityHandler;
    }

    void removeEntity(EntityHandler& entityHandler)
    {
        mPoolsManager.removeElement(entityHandler.mClassId, entityHandler.mSlot);
        entityHandler.reset();
    }

    Entity& getEntity(EntityHandler entityHandler) const
    {
        return mPoolsManager.getElementBase(entityHandler.mClassId, entityHandler.mSlot);
    }

private:
    PoolsManager<Entity> mPoolsManager;

public:
    CRGET(PoolsManager)
};
REGISTER_CLASS(EntityManager);