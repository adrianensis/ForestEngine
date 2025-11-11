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
