#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Entity::Entity()
{
    if (mEntityId == 0)
    {
        mEntityId = smEntityIdCounter++;
    }
}

void Entity::init()
{
	
}

void Entity::setIsActive(bool isActive)
{
	mIsActive = isActive;
}

void Entity::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	onDestroy();

	mIsDestroyed = true;
	mIsPendingToBeDestroyed = false;
}

void Entity::onRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
};

Entity& EntityPtr::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getEntitiesPool().getElementBase(*this);
}
NS_END