#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Entity::Entity()
{

}

void Entity::init()
{
	
}

void Entity::destroy()
{
	onDestroy();

	mIsDestroyed = true;
}

void Entity::onRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
};

Entity& EntityPtrBase::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getEntitiesPool().getElementBase(*this);
}
NS_END