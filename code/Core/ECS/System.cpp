#include "Core/ECS/System.hpp"

void System::registerComponentClass(ClassId classId)
{
    mAcceptedSystemComponentClasses.insert(classId);
}

bool System::isComponentClassAccepted(ClassId classId)
{
    return mAcceptedSystemComponentClasses.contains(classId);
}

void System::addSystemComponent(const ComponentHandler& component)
{
    CHECK_SYSTEM_COMPONENT(component);
    component.getComponent().mAlreadyAddedToSystem = true;
}

void System::removeSystemComponent(const ComponentHandler& component)
{
    CHECK_SYSTEM_COMPONENT(component);
    component.getComponent().mAlreadyAddedToSystem = false;
}

void SystemsManager::terminate()
{
    for (auto it = mSystemsInOrder.rbegin(); it != mSystemsInOrder.rend(); ++it)
    {
        (*it)->terminate();
        mSystems.erase(ClassManager::getDynamicClassMetadata<System>((*it).getInternalPointer()).mClassDefinition.getId());
    }

    mSystemsInOrder.clear();
}