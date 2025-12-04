#pragma once

#include "Core/Core.hpp"
#include "Core/HashedString/HashedString.hpp"

NS_BEGIN(EC)
class EntityComponentPool;
class EntityPtrBase;

class ComponentOwner
{
public:

    ComponentOwner() = default;
    ComponentOwner(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool)
    {
        mClassId = id;
        mSlot = slot;
        mECPool = ecPool;
    }

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
        mECPool = nullptr;
    }

public:
    Core::Slot mSlot;
    Core::ClassId mClassId = 0;
    Core::Ptr<EntityComponentPool> mECPool;
};

class Component
{	
public:
    Component();
    virtual ~Component();

	virtual void onECComponentAdded() { }
    virtual void onECComponentDestroyed() { };
    virtual void onECComponentRecycle(Core::Slot newSlot);

protected:
    ComponentOwner mComponentOwner;
	Core::Slot mSlot;
    
public:
    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    GET_SET(ComponentOwner)
	GET(Slot)
};
REGISTER_CLASS(Component);

class ComponentPtrBase
{
template<class T>
friend class ComponentPtr;
public:
    ComponentPtrBase()
    {
    }
    ComponentPtrBase(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): mPoolElement(id, slot)
    {
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = nullptr;
        #endif
    }
    ComponentPtrBase(const ComponentPtrBase& other): ComponentPtrBase(other.mPoolElement.mClassId, other.mPoolElement.mSlot, other.mECPool)
    {
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = other.mDebugPointer;
        #endif
    }

    template<class T> T_EXTENDS(T, Component)
    T& get() const
    {
        Component* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        CHECK_MSG(castedPointer, "Invalid Cast!");
        return *castedPointer;
    }

    Component* operator->() const { return &getInternal(); }

    bool isValid() const { return mPoolElement.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const ComponentPtrBase& other) const
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
    Component& getInternal() const;
    Core::Ptr<EntityComponentPool> mECPool;
    Core::PoolElementPtr mPoolElement;
public:
    #ifdef ENGINE_BUILD_DEBUG
    Component* mDebugPointer = nullptr;
    #endif
};

template<class T>// T_EXTENDS(T, Component)
class ComponentPtr : public ComponentPtrBase
{
public:
    ComponentPtr() = default;
    ComponentPtr(const T* component, Core::Ptr<EntityComponentPool> ecPool)
    {
        mECPool = ecPool;
        Core::ClassId id = Core::ClassManager::getDynamicClassMetadata(component).mClassDefinition.getId();
        mPoolElement = Core::PoolElementPtr(id, component->getSlot());
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = const_cast<T*>(component);
        #endif
        checkValid();
    }
    ComponentPtr(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): ComponentPtrBase(id, slot, ecPool)
    {
        checkValid();
    }

    ComponentPtr(const ComponentPtrBase& other): ComponentPtrBase(other)
    {
        checkValid();
    }
    T& get() const
    {
        return ComponentPtrBase::get<T>();
    }
    
    ComponentPtr& operator=(const ComponentPtrBase& other)
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

    void checkValid()
    {
        if(isValid())
        {
            Component* pointer = &getInternal();
            T* castedPointer = dynamic_cast<T*>(pointer);
            if(!castedPointer)
            {
                reset();
            }
        }
    }

    T* operator->() const { return &get(); }
    operator ComponentPtr<const T>() const { return ComponentPtr<const T>(mPoolElement.mClassId, mPoolElement.mSlot, mECPool); }
    template<class U> T_EXTENDS(T, U)
    operator ComponentPtr<U>() const { return ComponentPtr<U>(*this); }
};
NS_END