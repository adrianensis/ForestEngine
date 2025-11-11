#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/EntityComponentPool.hpp"

NS_BEGIN(EC)
Component::Component()
{
}

Component::~Component()
{

}

void Component::destroy()
{
	CHECK_MSG(!getIsDestroyed(), "Component already destroyed");
    mIsDestroyed = true;
    // mIsActive = false;
    onDestroy();
}

void Component::onRecycle(Core::Slot newSlot)
{
    mSlot = newSlot;
}

void Component::onDestroy()
{
}

Component& ComponentPtrBase::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mECPool->getComponentsPool().getElementBase(*this);
}
NS_END