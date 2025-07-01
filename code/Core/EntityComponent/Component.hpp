#pragma once

#include "Engine/Events/Event.hpp"
#include "Core/HashedString/HashedString.hpp"

class EntityPtr;

class Component: public ISerializable, public Event::IEventObject
{
	DECLARE_SERIALIZATION()
	
public:
    Component();
    virtual ~Component();

	virtual void onComponentAdded() { }

    bool isStatic() const;
    bool isActive() const;
    void setIsActive(bool isActive);
    void destroy();
    virtual void onDestroy();
    virtual void onRecycle(Core::Slot newSlot);

    // Important: Override this in ONLY those component classes allowed to be injected into engine systems
    // MeshRenderer for RenderEngine, Script for RenderEngine, ...
    // This will automatically work in derived classes, no need to override this method in derived classes
    virtual Core::ClassId getComponentTypeId() const { return 0; }

    EntityPtr getOwnerEntity() const;
    void setOwnerEntity(const EntityPtr& ownerEntity);

private:

class ComponentOwner
{
public:

    ComponentOwner() = default;
    ComponentOwner(Core::ClassId id, Core::Slot slot)
    {
        mClassId = id;
        mSlot = slot;
    }

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
    }

public:
    Core::Slot mSlot;
    Core::ClassId mClassId = 0;
};

public:
    bool mAlreadyAddedToSystem = false;

private:
	bool mIsActive = true;
	bool mIsDestroyed = false;
	Core::Slot mSlot;
	ComponentOwner mOwnerEntity;

	Core::u64 mComponentId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u64 smComponentIdCounter = 1;

    
public:
    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif

    GET(ComponentId)
	GET(IsDestroyed)
	GET(Slot)
};
REGISTER_CLASS(Component);


class ComponentPtr: public Core::PoolElementPtr
{
public:
    ComponentPtr(): Core::PoolElementPtr()
    {
    }
    ComponentPtr(Core::ClassId id, Core::Slot slot): Core::PoolElementPtr(id, slot)
    {
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = nullptr;
        #endif
    }
    ComponentPtr(const ComponentPtr& other): ComponentPtr(other.mClassId, other.mSlot)
    {
        #ifdef ENGINE_BUILD_DEBUG
        mDebugPointer = other.mDebugPointer;
        #endif
    }
    ComponentPtr(const Core::PoolElementPtr& other): ComponentPtr(other.mClassId, other.mSlot)
    {
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

public:
    #ifdef ENGINE_BUILD_DEBUG
    Component* mDebugPointer = nullptr;
    #endif
};

template<class T>// T_EXTENDS(T, Component)
class TComponentPtr : public ComponentPtr
{
public:
    TComponentPtr() = default;
    TComponentPtr(const T* component)
    {
        Core::ClassId id = Core::ClassManager::getDynamicClassMetadata(component).mClassDefinition.getId();
        *this = TComponentPtr(id, component->getSlot());
    }
    TComponentPtr(Core::ClassId id, Core::Slot slot): ComponentPtr(id, slot)
    {
        checkValid();
    }

    TComponentPtr(const ComponentPtr& other): ComponentPtr(other)
    {
        checkValid();
    }
    T& get() const
    {
        return ComponentPtr::get<T>();
    }
    
    TComponentPtr& operator=(const ComponentPtr& other)
    {
        if (this != &other)
        {
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
    operator TComponentPtr<const T>() const { return TComponentPtr<const T>(mClassId, mSlot); }
    template<class U> T_EXTENDS(T, U)
    operator TComponentPtr<U>() const { return TComponentPtr<U>(*this); }
};