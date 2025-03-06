#include "Core/EntityComponent/EntityPtr.hpp"
#include "Core/EntityComponent/EntityManager.hpp"

Entity& EntityPtr::getInternal() const
{
    CHECK_MSG(EntityPtr::isValid(), "Invalid handle!");
    return EntityManager::getInstance().getPoolsManager().getElementBase(mClassId, mSlot);
}

EntityPtr EntityPtr::getEntityPtr(ClassId id, const Entity& entity)
{
    return EntityPtr(id, entity.getSlot());
}
