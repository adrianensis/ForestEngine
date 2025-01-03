#include "Core/ECS/System.hpp"

void System::registerComponentClass(ClassId classId)
{
    mAcceptedSystemComponentClasses.insert(classId);
}

bool System::isComponentClassAccepted(ClassId classId) const
{
    return mAcceptedSystemComponentClasses.contains(classId);
}

void System::addSystemComponent(const ComponentHandler& component)
{
    checkSystemComponent(component);
    component.getComponent().mAlreadyAddedToSystem = true;
}

void System::removeSystemComponent(const ComponentHandler& component)
{
    checkSystemComponent(component);
    component.getComponent().mAlreadyAddedToSystem = false;
}

void System::checkSystemComponent(const ComponentHandler& component) const
{
    CHECK_MSG(component.isValid() && isComponentClassAccepted(component->getSystemComponentId()), "Component class is not valid for this system: " + std::to_string(component->getSystemComponentId()));
}
