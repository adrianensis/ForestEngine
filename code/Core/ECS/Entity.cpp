#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Component.hpp"
#include "Core/ECS/ComponentsManager.hpp"
#include "Core/ECS/EntityManager.hpp"
#include "Core/System/SystemManager.hpp"

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

void Entity::addComponentInternal(ComponentHandler componentHandler)
{
    PROFILER_CPU()
    CHECK_MSG(componentHandler.isValid(), "Invalid Component!");

	mComponentHandlers.emplace_back(componentHandler);
    Component& comp = componentHandler.getComponent();

    CHECK_MSG(comp.getOwnerEntity().isValid(), "invalid Entity!");
	comp.onComponentAdded();

    ComponentsManager::getInstance().notifyListenersOnComponentAdded(componentHandler);
}

void Entity::setComponentOwner(ComponentHandler componentHandler)
{
    PROFILER_CPU()
    CHECK_MSG(componentHandler.isValid(), "Invalid Component!");
    componentHandler->setOwnerEntity(EntityHandler::getEntityHandler(*this));
}

void Entity::removeComponent(ComponentHandler componentHandler)
{
    PROFILER_CPU()
    CHECK_MSG(componentHandler.isValid(), "Invalid Component!");
    CHECK_MSG(componentHandler.getComponent().getOwnerEntity().isValid(), "Component is not assigned to a Entity!");
    CHECK_MSG(componentHandler.getComponent().getOwnerEntity() == EntityHandler::getEntityHandler(*this), "Component is assigned to another Entity!");

    bool componentFound = false;
    FOR_LIST_COND(it, mComponentHandlers, !componentFound)
	{
        if((*it) == componentHandler)
        {
            componentFound = true;
            mComponentHandlers.erase(it);
        }
    }

    if(componentFound)
    {
        ComponentsManager::getInstance().notifyListenersOnComponentRemoved(componentHandler);
        componentHandler->destroy();
        ComponentsManager::getInstance().removeComponent(componentHandler);
    }
}

void Entity::setIsActive(bool isActive)
{
	mIsActive = mIsDestroyed || mIsPendingToBeDestroyed ? false : isActive;

	FOR_LIST(it, mComponentHandlers)
	// FOR_LIST(it, mComponents)
	{
		(*it).getComponent().setIsActive(isActive);
	}
}

void Entity::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	onDestroy();

	FOR_LIST(it, mComponentHandlers)
	{
        if((*it).isValid())
        {
            ComponentsManager::getInstance().notifyListenersOnComponentRemoved(*it);
            (*it).getComponent().destroy();
            ComponentsManager::getInstance().removeComponent(*it);
        }
	}

	mComponentHandlers.clear();
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