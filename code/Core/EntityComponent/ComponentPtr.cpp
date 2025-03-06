#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

Component& ComponentPtr::getInternal() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return ComponentsManager::getInstance().getPoolsManager().getElementBase(mClassId, mSlot);
}

ComponentPtr ComponentPtr::getComponentPtr(ClassId id, const Component& component)
{
    return ComponentPtr(id, component.getSlot());
}
