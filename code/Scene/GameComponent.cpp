#include "Scene/GameComponent.hpp"

EC::EntityPtrBase GameComponent::getOwnerEntity() const
{
    EC::EntityPtrBase entityPtr(mComponentOwner.mClassId, mComponentOwner.mSlot, mComponentOwner.mECPool);
    return entityPtr;
}
