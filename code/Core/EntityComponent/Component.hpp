#pragma once

#include "Core/Core.hpp"
#include "Core/HashedString/HashedString.hpp"

NS_BEGIN(EC)
class Entity;

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

class Component
{	
public:
    Component();
    virtual ~Component();

	virtual void onECComponentAdded() { }
    virtual void onECComponentDestroyed() { };
    virtual void onECComponentRecycle(Core::PoolElementPtr poolElementPtr);

protected:
    Entity* mComponentOwner = nullptr;
	Core::PoolElementPtr mPoolElementPtr;
    
public:
    #ifdef ENGINE_BUILD_DEBUG
    Core::HashedString mDebugString;
    #endif
    GET_SET(ComponentOwner)
	CRGET(PoolElementPtr)
};
REGISTER_CLASS(Component);
NS_END