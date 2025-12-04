#include "Scene/GameComponent.hpp"

GameComponent::GameComponent()
{
    if (mComponentId == 0)
    {
        mComponentId = smComponentIdCounter++;
    }
}

EC::Entity* GameComponent::getOwnerEntity() const
{
    return mComponentOwner;
}

bool GameComponent::isActive() const
{
	return mIsDestroyed ? false : mIsActive;
}

void GameComponent::setIsActive(bool isActive)
{
	mIsActive = isActive;
}

void GameComponent::onECComponentDestroyed()
{
    EC::Component::onECComponentDestroyed();

	CHECK_MSG(!getIsDestroyed(), "Component already destroyed");
    mIsDestroyed = true;
    // mIsActive = false;
    onDestroy();
    mComponentOwner = nullptr;
}

void GameComponent::onDestroy()
{
}
