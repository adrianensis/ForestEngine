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

void EngineSystemsManager::addComponentToEngineSystem(Ptr<EngineSystemComponent> component)
{
    if (component and !component->mAlreadyAddedToEngine)
    {
        ClassId componentClassId = component->getClassId();
        bool added = false;
        FOR_MAP(itEngineSystem, mEngineSystems)
        {
            Ptr<EngineSystem> sub = (itEngineSystem->second);
            if (sub->isComponentClassAccepted(componentClassId))
            {
                sub->addComponent(component);
                added = true;
            }
        }
    }
}