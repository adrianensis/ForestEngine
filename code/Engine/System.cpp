#include "Engine/System.hpp"


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