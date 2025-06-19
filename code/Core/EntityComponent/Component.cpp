#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/EntityManager.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

Component::Component()
{
    if (mComponentId == 0)
    {
        mComponentId = smComponentIdCounter++;
    }
}

Component::~Component()
{

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
    
    mOwnerEntity.reset();
}

void Component::onRecycle(Slot newSlot)
{
    mSlot = newSlot;
}

void Component::onDestroy()
{
}

EntityPtr Component::getOwnerEntity() const
{
    EntityPtr entityPtr(mOwnerEntity.mClassId, mOwnerEntity.mSlot);
    return entityPtr;
}

void Component::setOwnerEntity(const EntityPtr& ownerEntity)
{
    mOwnerEntity = ComponentOwner(ownerEntity.mClassId, ownerEntity.mSlot);
}

IMPLEMENT_SERIALIZATION(Component)
{
}

IMPLEMENT_DESERIALIZATION(Component)
{
 
}

Component& ComponentPtr::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return ComponentsManager::getInstance().getPool().getElementBase(*this);
}