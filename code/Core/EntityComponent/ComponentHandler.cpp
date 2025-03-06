#include "Core/EntityComponent/ComponentHandler.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

Component& ComponentHandler::getInternal() const
{
    CHECK_MSG(ComponentHandler::isValid(), "Invalid handler!");
    return mComponentsManager->getPoolsManager().getElementBase(mClassId, mSlot);
}

ComponentHandler ComponentHandler::getComponentHandler(ClassId id, const Component& component)
{
    return ComponentHandler(id, component.getSlot(), ComponentsManager::getInstance().getInstancePtr().getInternalPointer());
}
