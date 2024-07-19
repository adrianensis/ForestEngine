#include "Core/ECS/EntityHandler.hpp"
#include "Core/ECS/EntityManager.hpp"

Entity& EntityHandler::getInternal() const
{
    return mEntityManager->getGameObjectFromSlot(mClassId, mSlot);
}

EntityHandler EntityHandler::getEntityHandler(ClassId id, const Entity& entity)
{
    return GET_SYSTEM(EntityManager).getEntityHandler(id, entity);
}
