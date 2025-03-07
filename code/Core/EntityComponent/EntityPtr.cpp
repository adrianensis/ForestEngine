#include "Core/EntityComponent/EntityPtr.hpp"
#include "Core/EntityComponent/EntityManager.hpp"

Entity& EntityPtr::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return EntityManager::getInstance().getPool().getElementBase(mClassId, mSlot);
}
