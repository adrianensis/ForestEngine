#include "Core/ECS/ComponentHandler.hpp"
#include "Core/ECS/ComponentsManager.hpp"

Component& ComponentHandler::getInternal() const
{
    CHECK_MSG(ComponentHandler::isValid(), "Invalid handler!");
    return mComponentsManager->getComponentFromSlot(mClassId, mSlot);
}

ComponentHandler ComponentHandler::getComponentHandler(ClassId id, const Component& component)
{
    return GET_SYSTEM(ComponentsManager).getComponentHanlder(id, component);
}
