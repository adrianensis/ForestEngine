#include "Core/EntityComponent/ComponentsManager.hpp"

void ComponentsManager::notifyListenersOnComponentAdded(const ComponentPtr& componentPtr) const
{
    ClassId id = componentPtr->getComponentTypeId();
    if(mComponentListeners.contains(id))
    {
        FOR_LIST(it, mComponentListeners.at(id))
        {
            if((*it).isValid())
            {
                (*it)->onComponentAdded(componentPtr);
            }
        }
    }
}

void ComponentsManager::notifyListenersOnComponentRemoved(const ComponentPtr& componentPtr) const
{
    ClassId id = componentPtr->getComponentTypeId();
    if(mComponentListeners.contains(id))
    {
        FOR_LIST(it, mComponentListeners.at(id))
        {
            if((*it).isValid())
            {
                (*it)->onComponentRemoved(componentPtr);
            }
        }
    }
}