#pragma once

#include "Core/Core.hpp"
#include "Core/Memory/Pool.hpp"

NS_BEGIN(EC)
class EntityComponentPool;

class Entity
{
public:
    Entity();
    virtual void onECComponentRecycle(Core::Slot newSlot);
    
protected:
    Core::Slot mSlot;

public:

    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    
	GET(Slot)
};
REGISTER_CLASS(Entity);

class EntityPtrBase
{
template<class T>
friend class EntityPtr;
public:

    EntityPtrBase()
    {
    }
    EntityPtrBase(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): mPoolElement(id, slot)
    {
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = nullptr;
        #endif
    }
    EntityPtrBase(const EntityPtrBase& other): EntityPtrBase(other.mPoolElement.mClassId, other.mPoolElement.mSlot, other.mECPool)
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

    bool isValid() const { return mPoolElement.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const EntityPtrBase& other) const
	{
		return
         mPoolElement == other.mPoolElement;
	}

    void reset()
    {
        mPoolElement.reset();
    }

    const Core::PoolElementPtr& getPoolElement() const
    {
        return mPoolElement;
    }

    Core::Ptr<EntityComponentPool> getECPool() const
    {
        return mECPool;
    }

protected:
    Entity& getInternal() const;
    Core::Ptr<EntityComponentPool> mECPool;
    Core::PoolElementPtr mPoolElement;
public:
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
        mPoolElement = Core::PoolElementPtr(id, entity->getSlot());
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = const_cast<T*>(entity);
        #endif
        checkValid();
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
            mPoolElement = other.mPoolElement;
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
            mPoolElement == Core::PoolElementPtr(id, entity->getSlot());
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
    operator EntityPtr<const T>() const { return EntityPtr<const T>(mPoolElement.mClassId, mPoolElement.mSlot, mECPool); }
    template<class U> T_EXTENDS(T, U)
    operator EntityPtr<U>() const { return EntityPtr<U>(mPoolElement.mClassId, mPoolElement.mSlot, mECPool); }
};
NS_END