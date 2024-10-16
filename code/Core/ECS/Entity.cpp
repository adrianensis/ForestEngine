#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Component.hpp"
#include "Core/ECS/ComponentsManager.hpp"
#include "Core/ECS/EntityManager.hpp"

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

	SystemsManager::getInstance().addComponentToSystem(componentHandler);
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

    FOR_LIST(it, mComponentHandlers)
	{
        if((*it) == componentHandler)
        {
            SystemsManager::getInstance().removeComponentFromSystem(componentHandler);
            componentHandler->destroy();
            // mComponentHandlers.erase(it);
            GET_SYSTEM(ComponentsManager).removeComponent(*it);
            break;
        }
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
            SystemsManager::getInstance().removeComponentFromSystem(*it);
            (*it).getComponent().destroy();
            GET_SYSTEM(ComponentsManager).removeComponent(*it);
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