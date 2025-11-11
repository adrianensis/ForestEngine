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

EntityPtrBase Component::getOwnerEntity() const
{
    // TODO: Component::getOwnerEntity fix nullptr
    EntityPtrBase entityPtr(mOwnerEntity.mClassId, mOwnerEntity.mSlot, mOwnerEntity.mECPool);
    return entityPtr;
}

void Component::setOwnerEntity(const EntityPtrBase& ownerEntity)
{
    mOwnerEntity = ComponentOwner(ownerEntity.mClassId, ownerEntity.mSlot, ownerEntity.mECPool);
}

Component& ComponentPtrBase::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getComponentsPool().getElementBase(*this);
}
NS_END