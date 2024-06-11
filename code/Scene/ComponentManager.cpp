#include "Scene/ComponentsManager.hpp"
#include "Scene/GameObject.hpp"

void ComponentsManager::init() 
{
}

void ComponentsManager::terminate() 
{
}

Ptr<Component> ComponentHandler::getComponent() const
{
    CHECK_MSG(isValid(), "Invalid handler!");
    ComponentHandler componentHandler = *this;
    return mComponentsManager->getComponent(componentHandler);
}