#pragma once

#include "Core/Core.hpp"

NS_BEGIN(EC)
class EntityComponentPool;

class Entity
{
public:
    Entity();

    virtual void init();

	virtual void onDestroy(){};
    virtual void onRecycle(Core::Slot newSlot);
    
    void destroy();

protected:
	bool mIsDestroyed = false;
    Core::Slot mSlot;

public:

    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    
	GET(IsDestroyed)
	GET(Slot)
};
REGISTER_CLASS(Entity);

class EntityPtrBase: public Core::PoolElementPtr
{
template<class T>
friend class EntityPtr;
public:

    EntityPtrBase(): Core::PoolElementPtr()
    {
    }
    EntityPtrBase(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): Core::PoolElementPtr(id, slot)
    {
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = nullptr;
        #endif
    }
    EntityPtrBase(const EntityPtrBase& other): EntityPtrBase(other.mClassId, other.mSlot, other.mECPool)
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
class EntityPtr : public EntityPtrBase
{
public:
    EntityPtr() = default;
    EntityPtr(const T* entity, Core::Ptr<EntityComponentPool> ecPool)
    {
        mECPool = ecPool;
        Core::ClassId id = Core::ClassManager::getDynamicClassMetadata(entity).mClassDefinition.getId();
        *this = EntityPtr(id, entity->getSlot(), mECPool);
    }
    EntityPtr(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): EntityPtrBase(id, slot, ecPool)
    {
        checkValid();
    }

    EntityPtr(const EntityPtrBase& other): EntityPtrBase(other)
    {
        checkValid();
    }

    T& get() const
    {
        return EntityPtrBase::get<T>();
    }

    EntityPtr& operator=(const EntityPtrBase& other)
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
        return EntityPtr<T>::operator==(&entity);
	}

    bool operator==(const T* entity) const
	{
        Core::ClassId id = Core::ClassManager::getDynamicClassMetadata(entity).mClassDefinition.getId();
		return
            mClassId == id &&
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
    operator EntityPtr<const T>() const { return EntityPtr<const T>(mClassId, mSlot, mECPool); }
    template<class U> T_EXTENDS(T, U)
    operator EntityPtr<U>() const { return EntityPtr<U>(mClassId, mSlot, mECPool); }
};
NS_END