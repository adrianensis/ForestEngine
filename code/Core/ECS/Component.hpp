#pragma once

#include "Core/Minimal.hpp"
#include "Core/Object/ObjectBase.hpp"

class EntityHandler;

class Component: public ObjectBase
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
    virtual ClassId getSystemComponentId() const { return 0; }

    const EntityHandler& getOwnerEntity() const;
    void setOwnerEntity(const EntityHandler& ownerEntity);

public:
    bool mAlreadyAddedToSystem = false;

private:
	bool mIsActive = true;
	bool mIsDestroyed = false;
	Slot mSlot;
	EntityHandler* mOwnerEntity = nullptr;

public:
	GET(IsDestroyed)
	GET(Slot)
};
REGISTER_CLASS(Component);
