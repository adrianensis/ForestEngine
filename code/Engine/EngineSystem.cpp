#include "Engine/EngineSystem.hpp"


void EngineSystem::registerComponentClass(ClassId classId)
{
    mAcceptedEngineSystemComponentClasses.insert(classId);
}

bool EngineSystem::isComponentClassAccepted(ClassId classId)
{
    return mAcceptedEngineSystemComponentClasses.contains(classId);
}

void EngineSystem::addComponent(Ptr<EngineSystemComponent> component)
{
    component->mAlreadyAddedToEngine = true;
}