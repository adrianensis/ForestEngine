#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Component::Component()
{
}

Component::~Component()
{

}

void Component::onECComponentRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
}

Component& ComponentPtrBase::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getComponentsPool().getElementBase(this->mPoolElement);
}
NS_END