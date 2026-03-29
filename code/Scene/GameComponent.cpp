#include "Scene/GameComponent.hpp"
#include "Scene/GameObject.hpp"

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

GameObject* GameComponent::getOwnerGameObject() const
{
    return static_cast<GameObject*>(mComponentOwner);
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
