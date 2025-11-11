#include "Scene/GameComponent.hpp"

GameComponent::GameComponent()
{
    if (mComponentId == 0)
    {
        mComponentId = smComponentIdCounter++;
    }
}

EC::EntityPtrBase GameComponent::getOwnerEntity() const
{
    EC::EntityPtrBase entityPtr(mComponentOwner.mClassId, mComponentOwner.mSlot, mComponentOwner.mECPool);
    return entityPtr;
}

bool GameComponent::isActive() const
{
	return mIsDestroyed ? false : mIsActive;
}

void GameComponent::setIsActive(bool isActive)
{
	mIsActive = isActive;
}
