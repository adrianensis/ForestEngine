#include "Core/EngineSystem.hpp"


void EngineSystem::registerComponentClass(ClassId classId)
{
    mAcceptedEngineSystemComponentClasses.insert(classId);
}

bool EngineSystem::isComponentClassAccepted(ClassId classId)
{
    return mAcceptedEngineSystemComponentClasses.find(classId) != mAcceptedEngineSystemComponentClasses.end();
}

void EngineSystem::addComponent(Ptr<EngineSystemComponent> component)
{
    component->mAlreadyAddedToEngine = true;
}