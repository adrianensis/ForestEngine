#include "Core/ECS/Component.hpp"
#include "Core/ECS/EntityHandler.hpp"
#include "Core/ECS/EntityManager.hpp"

Component::Component()
{
    //mOwnerEntity = Memory::newObject<EntityHandler>();
}

Component::~Component()
{
    if(mOwnerEntity)
    {
        Memory::deleteObject<EntityHandler>(mOwnerEntity);
    }
}

bool Component::isStatic() const
{
	return getOwnerEntity()->mIsStatic;
}

bool Component::isActive() const
{
	return (mIsDestroyed || !getOwnerEntity()) ? false : mIsActive;
}

void Component::setIsActive(bool isActive)
{
	mIsActive = (mIsDestroyed || !getOwnerEntity()) ? false : isActive;
}

void Component::destroy()
{
	CHECK_MSG(!getIsDestroyed(), "Component already destroyed");
    mIsDestroyed = true;
    mIsActive = false;
    onDestroy();
    
    if(mOwnerEntity)
    {
        mOwnerEntity->reset();
    }
}

void Component::onRecycle(Slot newSlot)
{
    mSlot = newSlot;
}

void Component::onDestroy()
{
}

const EntityHandler& Component::getOwnerEntity() const
{
    return *mOwnerEntity;
}

void Component::setOwnerEntity(const EntityHandler& ownerEntity)
{
    if(!mOwnerEntity)
    {
        mOwnerEntity = Memory::newObject<EntityHandler>();
    }

    *mOwnerEntity = ownerEntity;
}

IMPLEMENT_SERIALIZATION(Component)
{
}

IMPLEMENT_DESERIALIZATION(Component)
{
 
}