#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
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
	return mIsStatic;
}

bool Component::isActive() const
{
	return mIsDestroyed ? false : mIsActive;
}

void Component::setIsActive(bool isActive)
{
	mIsActive = isActive;
}

void Component::destroy()
{
	CHECK_MSG(!getIsDestroyed(), "Component already destroyed");
    mIsDestroyed = true;
    mIsActive = false;
    onDestroy();
    
    mOwnerEntity.reset();
}

void Component::onRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
}

void Component::onDestroy()
{
}

EntityPtr Component::getOwnerEntity() const
{
    // TODO: Component::getOwnerEntity fix nullptr
    EntityPtr entityPtr(mOwnerEntity.mClassId, mOwnerEntity.mSlot, mOwnerEntity.mECPool);
    return entityPtr;
}

void Component::setOwnerEntity(const EntityPtr& ownerEntity)
{
    mOwnerEntity = ComponentOwner(ownerEntity.mClassId, ownerEntity.mSlot, ownerEntity.mECPool);
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
    return mECPool->getComponentsPool().getElementBase(*this);
}
NS_END