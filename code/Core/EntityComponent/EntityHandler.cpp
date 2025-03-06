#include "Core/EntityComponent/EntityHandler.hpp"
#include "Core/EntityComponent/EntityManager.hpp"

Entity& EntityHandler::getInternal() const
{
    CHECK_MSG(EntityHandler::isValid(), "Invalid handler!");
    return mEntityManager->getPoolsManager().getElementBase(mClassId, mSlot);
}

EntityHandler EntityHandler::getEntityHandler(ClassId id, const Entity& entity)
{
    return EntityHandler(id, entity.getSlot(), EntityManager::getInstance().getInstancePtr().getInternalPointer());
}
