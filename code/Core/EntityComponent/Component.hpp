#pragma once

#include "Core/Core.hpp"
#include "Core/HashedString/HashedString.hpp"

NS_BEGIN(EC)
class Entity;

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