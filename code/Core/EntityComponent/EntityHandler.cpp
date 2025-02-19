#include "Core/EntityComponent/EntityHandler.hpp"
#include "Core/EntityComponent/EntityManager.hpp"

Entity& EntityHandler::getInternal() const
{
    CHECK_MSG(EntityHandler::isValid(), "Invalid handler!");
    return mEntityManager->getEntityFromSlot(mClassId, mSlot);
}

EntityHandler EntityHandler::getEntityHandler(ClassId id, const Entity& entity)
{
    return EntityManager::getInstance().getEntityHandler(id, entity);
}
