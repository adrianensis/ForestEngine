#include "Core/ECS/ComponentsManager.hpp"
#include "Scene/GameObject.hpp"

void ComponentsManager::init() 
{
}

void ComponentsManager::terminate() 
{
}

Component& ComponentHandler::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handler!");
    return mComponentsManager->getComponentFromSlot(mClassId, mSlot);
}

ComponentHandler ComponentHandler::getComponentHandler(ClassId id, Component& component)
{
    return GET_SYSTEM(ComponentsManager).getComponentHanlder(id, component);
}
