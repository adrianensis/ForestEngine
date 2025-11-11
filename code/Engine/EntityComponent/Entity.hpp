#pragma once

#include "Core/Core.hpp"

NS_BEGIN(EC)
class EntityComponentPool;

class Entity
{
public:
    Entity();

    virtual void init();

	bool isActive() const
	{
		return (mIsDestroyed || mIsPendingToBeDestroyed) ? false : mIsActive;
	};

	virtual void onDestroy(){};
    virtual void onRecycle(Core::Slot newSlot);
    
    void setIsActive(bool isActive);
    void destroy();

private:
	bool mIsActive = true;

	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
    Core::Slot mSlot;

	Core::u64 mEntityId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u64 smEntityIdCounter = 1;

public:
	Core::HashedString mTag;

    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET(Slot)
	GET(EntityId)
};
REGISTER_CLASS(Entity);

class EntityPtr: public Core::PoolElementPtr
{
template<class T>
friend class TEntityPtr;
public:

    EntityPtr(): Core::PoolElementPtr()
    {
    }
    EntityPtr(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): Core::PoolElementPtr(id, slot)
    {
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = nullptr;
        #endif
    }
    EntityPtr(const EntityPtr& other): EntityPtr(other.mClassId, other.mSlot, other.mECPool)
    {
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = other.mDebugPointer;
        #endif
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
public:
    Core::Ptr<EntityComponentPool> mECPool;
    #ifdef ENGINE_BUILD_DEBUG
    Entity* mDebugPointer = nullptr;
    #endif
};

template<class T>// T_EXTENDS(T, Entity)
class TEntityPtr : public EntityPtr
{
public:
    TEntityPtr() = default;
    TEntityPtr(const T* entity, Core::Ptr<EntityComponentPool> ecPool)
    {
        mECPool = ecPool;
        Core::ClassId id = Core::ClassManager::getDynamicClassMetadata(entity).mClassDefinition.getId();
        *this = TEntityPtr(id, entity->getSlot(), mECPool);
    }
    TEntityPtr(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): EntityPtr(id, slot, ecPool)
    {
        checkValid();
    }

    TEntityPtr(const EntityPtr& other): EntityPtr(other)
    {
        checkValid();
    }

    T& get() const
    {
        return EntityPtr::get<T>();
    }

    TEntityPtr& operator=(const EntityPtr& other)
    {
        if (this != &other)
        {
            mECPool = other.mECPool;
            mClassId = other.mClassId;
            mSlot = other.mSlot;
            #ifdef ENGINE_BUILD_DEBUG
            mDebugPointer = other.mDebugPointer;
            #endif
            checkValid();
        }
        return *this;
    }

    bool operator==(const T& entity) const
	{
        TEntityPtr other(entity, nullptr);
		return
            mClassId == other.mClassId &&
            mSlot.getSlot() == entity->getSlot().getSlot();
	}

    bool operator==(const T* entity) const
	{
        TEntityPtr other(entity, nullptr);
		return
            mClassId == other.mClassId &&
            mSlot.getSlot() == entity->getSlot().getSlot();
	}

    void checkValid()
    {
        if(isValid())
        {
            Entity* pointer = &getInternal();
            #ifdef ENGINE_BUILD_DEBUG
            mDebugPointer = pointer;
            #endif
            T* castedPointer = dynamic_cast<T*>(pointer);
            if(!castedPointer)
            {
                reset();
            }
        }
    }
    
    T* operator->() const { return &get(); }
    operator TEntityPtr<const T>() const { return TEntityPtr<const T>(mClassId, mSlot, mECPool); }
    template<class U> T_EXTENDS(T, U)
    operator TEntityPtr<U>() const { return TEntityPtr<U>(mClassId, mSlot, mECPool); }
};
NS_END