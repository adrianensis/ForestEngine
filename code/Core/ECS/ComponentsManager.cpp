#include "Core/ECS/ComponentsManager.hpp"

void ComponentsManager::init() 
{
}

void ComponentsManager::terminate() 
{
}

void ComponentsManager::notifyListenersOnComponentAdded(const ComponentHandler& componentHandler) const
{
    ClassId id = componentHandler.mClassId;
    if(mComponentListeners.contains(id))
    {
        FOR_LIST(it, mComponentListeners.at(id))
        {
            if((*it).isValid())
            {
                (*it)->onComponentAdded(componentHandler);
            }
        }
    }
}

void ComponentsManager::notifyListenersOnComponentRemoved(const ComponentHandler& componentHandler) const
{
    ClassId id = componentHandler.mClassId;
    if(mComponentListeners.contains(id))
    {
        FOR_LIST(it, mComponentListeners.at(id))
        {
            if((*it).isValid())
            {
                (*it)->onComponentRemoved(componentHandler);
            }
        }
    }
}