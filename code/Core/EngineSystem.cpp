#include "Core/EngineSystem.hpp"


void EngineSystem::registerComponentClass(ClassId classId)
{
    mAcceptedEngineSystemComponentClasses.insert(classId);
}

bool EngineSystem::isComponentClassAccepted(ClassId classId)
{
    return mAcceptedEngineSystemComponentClasses.find(classId) != mAcceptedEngineSystemComponentClasses.end();
}

void EngineSystem::init()
{
    REGISTER_ENGINE_SYSTEM(getPtrToThis());
}

void EngineSystem::addComponent(Ptr<EngineSystemComponent> component)
{
    component.get().setAlreadyAddedToEngine(true);
}

void EngineSystemsManager::addComponentToEngineSystem(Ptr<EngineSystemComponent> component)
{
    if (component and !component.get().getAlreadyAddedToEngine())
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

void EngineSystemsManager::registerEngineSystem(Ptr<EngineSystem> engineSystem)
{
    std::string_view className = engineSystem.get().getClassName();
    ClassId classId = engineSystem.get().getClassId();
    if(!MAP_CONTAINS(mEngineSystems, classId))
    {
        MAP_INSERT(mEngineSystems, classId, engineSystem);
    }
}
