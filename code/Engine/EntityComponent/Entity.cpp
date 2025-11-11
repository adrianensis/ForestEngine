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
	mIsActive = mIsDestroyed || mIsPendingToBeDestroyed ? false : isActive;

	// TODO: restore
    // const auto& components = ECManager.getComponents(TEntityPtr(this));
	// FOR_LIST(it, components)
	// // FOR_LIST(it, mComponents)
	// {
	// 	(*it)->setIsActive(isActive);
	// }
}

void Entity::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	onDestroy();
	// TODO: restore
    // ECManager.removeComponents(TEntityPtr(this));
}

void Entity::onRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
};

IMPLEMENT_SERIALIZATION(Entity)
{

	SERIALIZE("is_static", mIsStatic)
	SERIALIZE("should_persist", mShouldPersist)

	// SERIALIZE("tag", mTag)
}

IMPLEMENT_DESERIALIZATION(Entity)
{
	DESERIALIZE("is_static", mIsStatic)
	DESERIALIZE("should_persist", mShouldPersist)

	// DESERIALIZE("tag", mTag)
}

Entity& EntityPtr::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getEntitiesPool().getElementBase(*this);
}
NS_END