#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

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

void Entity::addComponent(const ComponentPtr& componentPtr)
{
    PROFILER_CPU()
    CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
    CHECK_MSG(!componentPtr->getOwnerEntity().isValid(), "Component is assigned to another Entity!");
    CHECK_MSG(componentPtr->getOwnerEntity() != TEntityPtr(this), "Component is already assigned to Entity!");

    componentPtr->setOwnerEntity(TEntityPtr(this));
    CHECK_MSG(componentPtr->getOwnerEntity().isValid(), "invalid Entity!");

	mComponentPtrs.emplace_back(componentPtr);
	componentPtr->onComponentAdded();

    EntityComponentManager::getInstance().notifyListenersOnComponentAdded(componentPtr);
}

void Entity::removeComponent(ComponentPtr& componentPtr)
{
    PROFILER_CPU()
    CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
    CHECK_MSG(componentPtr->getOwnerEntity().isValid(), "Component is not assigned to a Entity!");
    CHECK_MSG(componentPtr->getOwnerEntity() == TEntityPtr(this), "Component is assigned to another Entity!");

    bool componentFound = false;
    FOR_LIST(it, mComponentPtrs)
	{
        if((*it) == componentPtr)
        {
            componentFound = true;
            mComponentPtrs.erase(it);
            break;
        }
    }

    if(componentFound)
    {
        EntityComponentManager::getInstance().notifyListenersOnComponentRemoved(componentPtr);
        componentPtr->destroy();
        EntityComponentManager::getInstance().removeComponent(componentPtr);
    }
}

void Entity::setIsActive(bool isActive)
{
	mIsActive = mIsDestroyed || mIsPendingToBeDestroyed ? false : isActive;

	FOR_LIST(it, mComponentPtrs)
	// FOR_LIST(it, mComponents)
	{
		(*it)->setIsActive(isActive);
	}
}

void Entity::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	onDestroy();

	FOR_LIST(it, mComponentPtrs)
	{
        if((*it).isValid())
        {
            EntityComponentManager::getInstance().notifyListenersOnComponentRemoved(*it);
            (*it)->destroy();
            EntityComponentManager::getInstance().removeComponent(*it);
        }
	}

	mComponentPtrs.clear();
}

void Entity::onRecycle(Slot newSlot)
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
    return EntityComponentManager::getInstance().getEntitiesPool().getElementBase(*this);
}