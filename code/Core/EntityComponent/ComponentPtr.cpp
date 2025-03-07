#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

Component& ComponentPtr::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return ComponentsManager::getInstance().getPool().getElementBase(mClassId, mSlot);
}