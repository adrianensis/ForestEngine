#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/Entity.hpp"

class EntityManager;

class EntityHandler
{
public:

    EntityHandler() = default;
    EntityHandler(ClassId id, Slot slot, EntityManager* entityManager)
    {
        mClassId = id;
        mSlot = slot;
        mEntityManager = entityManager;
    }

    EntityHandler(ClassId id, Slot slot, const EntityManager* entityManager): EntityHandler(id, slot, const_cast<EntityManager*>(entityManager))
    {
    }

    EntityHandler(const EntityHandler& other): EntityHandler(other.mClassId, other.mSlot, other.mEntityManager)
    {
    }

    virtual ~EntityHandler()
    {
        reset();
    }

    template<class T> T_EXTENDS(T, Entity)
    T& get() const
    {
        Entity* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        CHECK_MSG(castedPointer, "Invalid Cast!");
        return *castedPointer;
    }

    Entity& getEntity() const
    {
        return get<Entity>();
    }

    template<class T> T_EXTENDS(T, Entity)
    static EntityHandler getEntityHandler(T& entity)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(&entity).mClassDefinition.getId();
        return getEntityHandler(id, entity);
    }
    static EntityHandler getEntityHandler(ClassId id, const Entity& entity);

    EntityHandler& operator=(const EntityHandler& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
            mEntityManager = other.mEntityManager;
        }
        return *this;
    }

    Entity* operator->() const { return getEntityPointer(); }
    virtual bool isValid() const { return mEntityManager && mClassId > 0 && mSlot.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const EntityHandler& other) const
	{
		return
         mEntityManager == other.mEntityManager &&
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
        mEntityManager = nullptr;
    }

protected:
    Entity* getEntityPointer() const
    {
        return& getInternal();
    }

    Entity& getInternal() const;

public:
    Slot mSlot;
    ClassId mClassId = 0;
    EntityManager* mEntityManager = nullptr;
};

template<class T>// T_EXTENDS(T, Entity)
class TypedEntityHandler : public EntityHandler
{
public:
    TypedEntityHandler() = default;
    TypedEntityHandler(ClassId id, Slot slot, EntityManager* entityManager): EntityHandler(id, slot, entityManager)
    {
    }

    TypedEntityHandler(ClassId id, Slot slot, const EntityManager* entityManager): TypedEntityHandler(id, slot, const_cast<EntityManager*>(entityManager))
    {
    }

    TypedEntityHandler(const EntityHandler& other): TypedEntityHandler(other.mClassId, other.mSlot, other.mEntityManager)
    {
    }

    T& get() const
    {
        return EntityHandler::get<T>();
    }
    
    T* operator->() const { return &get(); }
    virtual bool isValid() const override
    {
        if(!mEntityManager) {return false;}

        Entity* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        return mClassId > 0 && mSlot.isValid() && castedPointer;
    }
    operator TypedEntityHandler<const T>() const { return TypedEntityHandler<const T>(mClassId, mSlot, mEntityManager); }
    template<class U> T_EXTENDS(T, U)
    operator TypedEntityHandler<U>() const { return TypedEntityHandler<U>(mClassId, mSlot, mEntityManager); }
};