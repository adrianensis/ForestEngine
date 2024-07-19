#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
#include "Core/ECS/EntityHandler.hpp"

class EntityManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    template<class T> T_EXTENDS(T, Entity)
    TypedEntityHandler<T> requestEntity()
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mEntitiesArrays.contains(id))
        {
            mEntitiesArrays.emplace(id, OwnerPtr<EntitiesArrayBase>::moveCast(OwnerPtr<EntitiesArray<T>>::newObject(smInitialEntities)));
        }

        if(mEntitiesArrays.at(id)->mSlotsManager.isEmpty())
        {
            CHECK_MSG(false, "No space available for Entities!");
            // mEntitiesArrays.at(id).mSlotsManager.increaseSize(smInitialEntities);
            // mEntitiesArrays.at(id).mEntities.resize(mEntitiesArrays.at(id).mSlotsManager.getSize());
        }

        TypedEntityHandler<T> entityHandler(id, mEntitiesArrays.at(id)->mSlotsManager.requestSlot(), this);
        if(entityHandler.isValid())
        {
            Entity& entity = mEntitiesArrays.at(id)->at(entityHandler.mSlot.getSlot());
            Memory::registerPointer<T>(static_cast<T*>(&entity));
            entity.onRecycle(entityHandler.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }

        return entityHandler;
    }

    void removeGameObject(EntityHandler& entityHandler)
    {
        ClassId id = entityHandler.mClassId;

        if(mEntitiesArrays.contains(id))
        {
            mEntitiesArrays.at(id)->mSlotsManager.freeSlot(entityHandler.mSlot);
        }
        Memory::unregisterPointer(&entityHandler.getEntity());
        entityHandler.reset();
    }

    Entity& getGameObject(EntityHandler entityHandler) const
    {
        u32 slot = entityHandler.mSlot.getSlot();
        return mEntitiesArrays.at(entityHandler.mClassId)->at(slot);
    }

    Entity& getGameObjectFromSlot(ClassId classId, const Slot& slot) const
    {
        return mEntitiesArrays.at(classId)->at(slot.getSlot());
    }

    EntityHandler getEntityHandler(ClassId id, const Entity& entity)
    {
        EntityHandler entityHandler(id, entity.getSlot(), this);
        return entityHandler;
    }

private:
    class EntitiesArrayBase
    {
    public:
        virtual ~EntitiesArrayBase() = default;
        EntitiesArrayBase(u32 reservedGameObjects)
        {
            mSlotsManager.init(reservedGameObjects);
        }
        virtual Entity& at(u32 index) = 0;
        SlotsManager mSlotsManager;
    };
    template <class T> T_EXTENDS(T, Entity)
    class EntitiesArray : public EntitiesArrayBase
    {
    public:
        EntitiesArray(u32 reservedGameObjects) : EntitiesArrayBase(reservedGameObjects)
        {
            mGameObjects.resize(reservedGameObjects);
            mSlotsManager.init(reservedGameObjects);
        }
        virtual Entity& at(u32 index) override
        {
            return mGameObjects.at(index);
        }
        std::vector<T> mGameObjects;
    };

    std::unordered_map<ClassId, OwnerPtr<EntitiesArrayBase>> mEntitiesArrays;

    inline static const u32 smInitialEntities = 1000000;
};
REGISTER_CLASS(EntityManager);