#pragma once

#include "Core/Events/Event.hpp"

class Entity: public ISerializable, public IEventObject
{
    
	DECLARE_SERIALIZATION()
	
public:
    Entity();

    virtual void init();

	bool isActive() const
	{
		return (mIsDestroyed || mIsPendingToBeDestroyed) ? false : mIsActive;
	};

	void finallyDestroy()
	{
		mIsDestroyed = true;
		mIsPendingToBeDestroyed = false;
	};

	virtual void onDestroy(){};
    virtual void onRecycle(Slot newSlot);
    
    void setIsActive(bool isActive);
    void destroy();

private:
	bool mIsActive = true;

	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
    Slot mSlot;

	u64 mEntityId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static u64 smEntityIdCounter = 1;

public:
	bool mIsStatic = false;
	HashedString mTag;
	bool mShouldPersist = false;
    
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET(Slot)
	GET(EntityId)
};
REGISTER_CLASS(Entity);

class EntityPtr: public PoolElementPtr
{
public:

    EntityPtr(): PoolElementPtr()
    {
    }
    EntityPtr(ClassId id, Slot slot): PoolElementPtr(id, slot)
    {
    }

    EntityPtr(const EntityPtr& other): PoolElementPtr(other)
    {
    }
    EntityPtr(const PoolElementPtr& other): PoolElementPtr(other)
    {
    }

    template<class T> T_EXTENDS(T, Entity)
    T& get() const
    {
        Entity* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        CHECK_MSG(castedPointer, "Invalid Cast!");
        return *castedPointer;
    }

    Entity* operator->() const { return &getInternal(); }

protected:
    Entity& getInternal() const;
};

template<class T>// T_EXTENDS(T, Entity)
class TEntityPtr : public EntityPtr
{
public:
    TEntityPtr() = default;
    TEntityPtr(const T* entity)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(entity).mClassDefinition.getId();
        *this = TEntityPtr(id, entity->getSlot());
    }
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

    bool operator==(const T& entity) const
	{
        TEntityPtr other(entity);
		return
            mClassId == other.mClassId &&
            mSlot.getSlot() == other.mSlot.getSlot();
	}

    bool operator==(const T* entity) const
	{
        TEntityPtr other(entity);
		return
            mClassId == other.mClassId &&
            mSlot.getSlot() == other.mSlot.getSlot();
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