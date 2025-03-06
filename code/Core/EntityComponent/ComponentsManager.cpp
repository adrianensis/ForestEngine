#include "Core/EntityComponent/ComponentsManager.hpp"

void ComponentsManager::notifyListenersOnComponentAdded(const ComponentHandler& componentHandler) const
{
    ClassId id = componentHandler->getComponentTypeId();
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
    ClassId id = componentHandler->getComponentTypeId();
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