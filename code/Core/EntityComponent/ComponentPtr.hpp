#pragma once

#include "Core/EntityComponent/Component.hpp"

class ComponentsManager;

class ComponentPtr
{
public:
    ComponentPtr() = default;
    ComponentPtr(ClassId id, Slot slot, ComponentsManager* componentsManager)
    {
        mClassId = id;
        mSlot = slot;
        mComponentsManager = componentsManager;
    }

    ComponentPtr(ClassId id, Slot slot, const ComponentsManager* componentsManager): ComponentPtr(id, slot, const_cast<ComponentsManager*>(componentsManager))
    {
    }

    ComponentPtr(const ComponentPtr& other): ComponentPtr(other.mClassId, other.mSlot, other.mComponentsManager)
    {
    }

    virtual ~ComponentPtr()
    {
        reset();
    }

    ComponentPtr& operator=(const ComponentPtr& other)
    {
        if (other.isValid() && this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
            mComponentsManager = other.mComponentsManager;
            CHECK_MSG(isValid(), "Invalid handle!");
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

    virtual bool isValid() const { return mComponentsManager && mClassId > 0 && mSlot.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const ComponentPtr& other) const
	{
		return
         mComponentsManager == other.mComponentsManager &&
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
        mComponentsManager = nullptr;
    }

    template<class T> T_EXTENDS(T, Component)
    static ComponentPtr getComponentPtr(T& component)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(&component).mClassDefinition.getId();
        return getComponentPtr(id, component);
    }
    static ComponentPtr getComponentPtr(ClassId id, const Component& component);

protected:
    Component& getInternal() const;

public:
    Slot mSlot;
    ClassId mClassId = 0;
    ComponentsManager* mComponentsManager = nullptr;
};

template<class T>// T_EXTENDS(T, Component)
class TComponentPtr : public ComponentPtr
{
public:
    TComponentPtr() = default;
    TComponentPtr(ClassId id, Slot slot, ComponentsManager* componentsManager): ComponentPtr(id, slot, componentsManager)
    {
    }

    TComponentPtr(ClassId id, Slot slot, const ComponentsManager* componentsManager): TComponentPtr(id, slot, const_cast<ComponentsManager*>(componentsManager))
    {
    }

    TComponentPtr(const ComponentPtr& other): TComponentPtr(other.mClassId, other.mSlot, other.mComponentsManager)
    {
    }
    T& get() const
    {
        return ComponentPtr::get<T>();
    }
    
    T* operator->() const { return &get(); }
    virtual bool isValid() const override
    {
        if(!mComponentsManager) {return false;}

        Component* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        return mClassId > 0 && mSlot.isValid() && castedPointer;
    }
    operator TComponentPtr<const T>() const { return TComponentPtr<const T>(mClassId, mSlot, mComponentsManager); }
    template<class U> T_EXTENDS(T, U)
    operator TComponentPtr<U>() const { return TComponentPtr<U>(*this); }
};