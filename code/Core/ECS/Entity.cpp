#include "Core/ECS/Entity.hpp"
#include "Core/ECS/Component.hpp"
#include "Core/ECS/ComponentsManager.hpp"
#include "Core/ECS/EntityManager.hpp"

Entity::Entity()
{
	mIsActive = true;
	mShouldPersist = false;
}

void Entity::init()
{
	mTag = "";
}

void Entity::addComponentInternal(ComponentHandler componentHandler)
{
    CHECK_MSG(componentHandler.isValid(), "Invalid Component!");
    CHECK_MSG(!componentHandler.getComponent().getOwnerEntity().isValid(), "Component is already assigned to a Entity!");

	mComponentHandlers.emplace_back(componentHandler);
    Component& comp = componentHandler.getComponent();

	comp.setOwnerEntity(EntityHandler::getEntityHandler(*this));
    CHECK_MSG(comp.getOwnerEntity().isValid(), "invalid Entity!");
	comp.onComponentAdded();

	SystemsManager::getInstance().addComponentToSystem(componentHandler);
}

void Entity::removeComponent(ComponentHandler componentHandler)
{
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
    // FOR_LIST(it, mComponents)
	// {
    //     if(*it)
    //     {
    //         SystemsManager::getInstance().removeComponentFromSystem(Ptr<Component>(*it));
    //         (*it)->destroy();
    //     }
	// }

	// mComponents.clear();
}

void Entity::onRecycle(Slot newSlot)
{
    mSlot = newSlot;
    mIsActive = false;
    mIsPendingToBeDestroyed = false;
    mIsDestroyed = false;
};

IMPLEMENT_SERIALIZATION(Entity)
{

	SERIALIZE("is_static", mIsStatic)
	SERIALIZE("should_persist", mShouldPersist)

	SERIALIZE("tag", mTag)
}

IMPLEMENT_DESERIALIZATION(Entity)
{
	DESERIALIZE("is_static", mIsStatic)
	DESERIALIZE("should_persist", mShouldPersist)

	DESERIALIZE("tag", mTag)
}