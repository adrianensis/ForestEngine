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
            mEntitiesArrays.emplace(id, OwnerPtr<EntitiesArrayBase>::moveCast(OwnerPtr<EntitiesArray<T>>::newObject(smMaxEntities)));
        }

        if(mEntitiesArrays.at(id)->size() == smMaxEntities)
        {
            CHECK_MSG(false, "No space available for Entities!");
            // mEntitiesArrays.at(id).mSlotsManager.increaseSize(smInitialEntities);
            // mEntitiesArrays.at(id).mEntities.resize(mEntitiesArrays.at(id).mSlotsManager.getSize());
        }

        EntityHandler entityHandler(id, mEntitiesArrays.at(id)->mSlotsManager.requestSlot(), this);
        if(entityHandler.isValid())
        {
            if(entityHandler.mSlot.getSlot() == mEntitiesArrays.at(id)->size())
            {
                mEntitiesArrays.at(id)->emplaceBack();
            }

            Entity& entity = mEntitiesArrays.at(id)->at(entityHandler.mSlot.getSlot());
            T* entityT = static_cast<T*>(&entity);
            *entityT = T();
            Memory::registerPointer<T>(entityT);
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
        virtual u32 size() const = 0;
        virtual void emplaceBack() = 0;
        SlotsManager mSlotsManager;
    };
    template <class T> T_EXTENDS(T, Entity)
    class EntitiesArray : public EntitiesArrayBase
    {
    public:
        EntitiesArray(u32 reservedGameObjects) : EntitiesArrayBase(reservedGameObjects)
        {
            mEntities.reserve(reservedGameObjects);
            mSlotsManager.init(reservedGameObjects);
        }
        virtual Entity& at(u32 index) override
        {
            return mEntities.at(index);
        }
        virtual u32 size() const override
        {
            return mEntities.size();
        }
        virtual void emplaceBack() override
        {
            mEntities.emplace_back();
        }
        std::vector<T> mEntities;
    };

    std::unordered_map<ClassId, OwnerPtr<EntitiesArrayBase>> mEntitiesArrays;

    inline static const u32 smMaxEntities = 100000;
};
REGISTER_CLASS(EntityManager);