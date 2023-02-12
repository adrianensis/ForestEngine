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
    component.get().mAlreadyAddedToEngine = true;
}

void EngineSystemsManager::addComponentToEngineSystem(Ptr<EngineSystemComponent> component)
{
    if (component and !component.get().mAlreadyAddedToEngine)
    {
        ClassId componentClassId = component.get().getClassId();
        bool added = false;
        FOR_MAP(itEngineSystem, mEngineSystems)
        {
            Ptr<EngineSystem> sub = (itEngineSystem->second);
            if (sub.get().isComponentClassAccepted(componentClassId))
            {
                sub.get().addComponent(component);
                added = true;
            }
        }
    }
}