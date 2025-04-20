#pragma once

#include "Core/Events/Event.hpp"

class EntityPtr;

class Component: public ISerializable, public IEventObject
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
    virtual void onRecycle(Slot newSlot);

    // Important: Override this in ONLY those component classes allowed to be injected into engine systems
    // MeshRenderer for RenderEngine, Script for RenderEngine, ...
    // This will automatically work in derived classes, no need to override this method in derived classes
    virtual ClassId getComponentTypeId() const { return 0; }

    EntityPtr getOwnerEntity() const;
    void setOwnerEntity(const EntityPtr& ownerEntity);

private:

class ComponentOwner
{
public:

    ComponentOwner() = default;
    ComponentOwner(ClassId id, Slot slot)
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
    Slot mSlot;
    ClassId mClassId = 0;
};

public:
    bool mAlreadyAddedToSystem = false;

private:
	bool mIsActive = true;
	bool mIsDestroyed = false;
	Slot mSlot;
	ComponentOwner mOwnerEntity;

	u64 mComponentId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static u64 smComponentIdCounter = 1;

public:
    GET(ComponentId)

public:
	GET(IsDestroyed)
	GET(Slot)
};
REGISTER_CLASS(Component);


class ComponentPtr
{
public:
    ComponentPtr() = default;
    ComponentPtr(ClassId id, Slot slot)
    {
        mClassId = id;
        mSlot = slot;
    }

    ComponentPtr(const ComponentPtr& other): ComponentPtr(other.mClassId, other.mSlot)
    {
    }

    virtual ~ComponentPtr()
    {
        reset();
    }

    ComponentPtr& operator=(const ComponentPtr& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
        }
        return *this;
    }

    template<class T> T_EXTENDS(T, Component)
    T& get() const
    {
        Component* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        CHECK_MSG(castedPointer, "Invalid Cast!");
        return *castedPointer;
    }

    Component& getComponent() const
    {
        return get<Component>();
    }

    Component* operator->() const { return &getComponent(); }

    bool isValid() const { return mClassId > 0 && mSlot.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const ComponentPtr& other) const
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
    Component& getInternal() const;

public:
    Slot mSlot;
    ClassId mClassId = 0;
};

template<class T>// T_EXTENDS(T, Component)
class TComponentPtr : public ComponentPtr
{
public:
    TComponentPtr() = default;
    TComponentPtr(const T* component)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(component).mClassDefinition.getId();
        *this = TComponentPtr(id, component->getSlot());
    }
    TComponentPtr(ClassId id, Slot slot): ComponentPtr(id, slot)
    {
        checkValid();
    }

    TComponentPtr(const ComponentPtr& other): TComponentPtr(other.mClassId, other.mSlot)
    {
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