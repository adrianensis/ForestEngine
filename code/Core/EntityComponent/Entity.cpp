#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Entity::Entity()
{

}

void Entity::onECComponentRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
};

Entity& EntityPtrBase::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getEntitiesPool().getElementBase(*this);
}
NS_END