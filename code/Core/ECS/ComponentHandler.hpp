#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/Component.hpp"

class ComponentsManager;

class ComponentHandler
{
public:
    ComponentHandler() = default;
    ComponentHandler(ClassId id, Slot slot, ComponentsManager* componentsManager)
    {
        mClassId = id;
        mSlot = slot;
        mComponentsManager = componentsManager;
    }

    ComponentHandler(ClassId id, Slot slot, const ComponentsManager* componentsManager): ComponentHandler(id, slot, const_cast<ComponentsManager*>(componentsManager))
    {
    }

    ComponentHandler(const ComponentHandler& other): ComponentHandler(other.mClassId, other.mSlot, other.mComponentsManager)
    {
    }

    virtual ~ComponentHandler()
    {
        reset();
    }

    ComponentHandler& operator=(const ComponentHandler& other)
    {
        if (other.isValid() && this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
            mComponentsManager = other.mComponentsManager;
            CHECK_MSG(isValid(), "Invalid handler!");
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
    bool operator==(const ComponentHandler& other) const
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
    static ComponentHandler getComponentHandler(T& component)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(&component).mClassDefinition.getId();
        return getComponentHandler(id, component);
    }
    static ComponentHandler getComponentHandler(ClassId id, const Component& component);

protected:
    Component& getInternal() const;

public:
    Slot mSlot;
    ClassId mClassId = 0;
    ComponentsManager* mComponentsManager = nullptr;
};

template<class T>// T_EXTENDS(T, Component)
class TypedComponentHandler : public ComponentHandler
{
public:
    TypedComponentHandler() = default;
    TypedComponentHandler(ClassId id, Slot slot, ComponentsManager* componentsManager): ComponentHandler(id, slot, componentsManager)
    {
    }

    TypedComponentHandler(ClassId id, Slot slot, const ComponentsManager* componentsManager): TypedComponentHandler(id, slot, const_cast<ComponentsManager*>(componentsManager))
    {
    }

    TypedComponentHandler(const ComponentHandler& other): TypedComponentHandler(other.mClassId, other.mSlot, other.mComponentsManager)
    {
    }
    T& get() const
    {
        return ComponentHandler::get<T>();
    }
    
    T* operator->() const { return &get(); }
    virtual bool isValid() const override
    {
        if(!mComponentsManager) {return false;}

        Component* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        return mClassId > 0 && mSlot.isValid() && castedPointer;
    }
    operator TypedComponentHandler<const T>() const { return TypedComponentHandler<const T>(mClassId, mSlot, mComponentsManager); }
    template<class U> T_EXTENDS(T, U)
    operator TypedComponentHandler<U>() const { return TypedComponentHandler<U>(*this); }
};