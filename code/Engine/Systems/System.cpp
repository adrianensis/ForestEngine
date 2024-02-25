#include "Engine/Systems/System.hpp"


void System::registerComponentClass(ClassId classId)
{
    mAcceptedSystemComponentClasses.insert(classId);
}

bool System::isComponentClassAccepted(ClassId classId)
{
    return mAcceptedSystemComponentClasses.contains(classId);
}

void System::addComponent(Ptr<SystemComponent> component)
{
    component->mAlreadyAddedToSystem = true;
}

void System::removeComponent(Ptr<SystemComponent> component)
{
    component->mAlreadyAddedToSystem = false;
}