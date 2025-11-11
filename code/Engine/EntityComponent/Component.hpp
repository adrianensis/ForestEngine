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

	virtual void onComponentAdded() { }

    void destroy();
    virtual void onDestroy();
    virtual void onRecycle(Core::Slot newSlot);

    // Important: Override this in ONLY those component classes allowed to be injected into engine systems
    // MeshRenderer for RenderEngine, Script for RenderEngine, ...
    // This will automatically work in derived classes, no need to override this method in derived classes
    virtual Core::ClassId getComponentTypeId() const { return 0; }

protected:
    ComponentOwner mComponentOwner;
	bool mIsDestroyed = false;
	Core::Slot mSlot;
    
public:
    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    GET_SET(ComponentOwner)
	GET(IsDestroyed)
	GET(Slot)
};
REGISTER_CLASS(Component);

class ComponentPtrBase: public Core::PoolElementPtr
{
template<class T>
friend class ComponentPtr;
public:
    ComponentPtrBase(): Core::PoolElementPtr()
    {
    }
    ComponentPtrBase(Core::ClassId id, Core::Slot slot, Core::Ptr<EntityComponentPool> ecPool): Core::PoolElementPtr(id, slot)
    {
        mECPool = ecPool;
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = nullptr;
        #endif
    }
    ComponentPtrBase(const ComponentPtrBase& other): ComponentPtrBase(other.mClassId, other.mSlot, other.mECPool)
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

protected:
    Component& getInternal() const;
    Core::Ptr<EntityComponentPool> mECPool;
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
        *this = ComponentPtr(id, component->getSlot(), mECPool);
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
            mClassId = other.mClassId;
            mSlot = other.mSlot;
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
    operator ComponentPtr<const T>() const { return ComponentPtr<const T>(mClassId, mSlot); }
    template<class U> T_EXTENDS(T, U)
    operator ComponentPtr<U>() const { return ComponentPtr<U>(*this); }
};
NS_END