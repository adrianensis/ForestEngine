#pragma once

#include "Core/EntityComponent/Entity.hpp"

class EntityPtr
{
public:

    EntityPtr() = default;
    EntityPtr(ClassId id, Slot slot)
    {
        mClassId = id;
        mSlot = slot;
    }

    EntityPtr(const EntityPtr& other): EntityPtr(other.mClassId, other.mSlot)
    {
    }

    virtual ~EntityPtr()
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
    static EntityPtr getEntityPtr(T& entity)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(&entity).mClassDefinition.getId();
        return getEntityPtr(id, entity);
    }
    static EntityPtr getEntityPtr(ClassId id, const Entity& entity);

    EntityPtr& operator=(const EntityPtr& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
        }
        return *this;
    }

    Entity* operator->() const { return getEntityPointer(); }
    bool isValid() const { return mClassId > 0 && mSlot.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const EntityPtr& other) const
	{
		return
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
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
};

template<class T>// T_EXTENDS(T, Entity)
class TEntityPtr : public EntityPtr
{
public:
    TEntityPtr() = default;
    TEntityPtr(ClassId id, Slot slot): EntityPtr(id, slot)
    {
        checkValid();
    }

    TEntityPtr(const EntityPtr& other): TEntityPtr(other.mClassId, other.mSlot)
    {
    }

    T& get() const
    {
        return EntityPtr::get<T>();
    }

    TEntityPtr& operator=(const EntityPtr& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;

            checkValid();
        }
        return *this;
    }

    void checkValid()
    {
        if(isValid())
        {
            Entity* pointer = &getInternal();
            T* castedPointer = dynamic_cast<T*>(pointer);
            if(!castedPointer)
            {
                reset();
            }
        }
    }
    
    T* operator->() const { return &get(); }
    operator TEntityPtr<const T>() const { return TEntityPtr<const T>(mClassId, mSlot); }
    template<class U> T_EXTENDS(T, U)
    operator TEntityPtr<U>() const { return TEntityPtr<U>(mClassId, mSlot); }
};