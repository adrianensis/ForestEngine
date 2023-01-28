#include "Core/EngineSystem.hpp"


void IEngineSystem::registerComponentClass(ClassId classId)
{
    mAcceptedEngineSystemComponentClasses.insert(classId);
}

bool IEngineSystem::isComponentClassAccepted(ClassId classId)
{
    return mAcceptedEngineSystemComponentClasses.find(classId) != mAcceptedEngineSystemComponentClasses.end();
}

void IEngineSystem::init()
{
    REGISTER_ENGINE_SYSTEM(this);
}

void IEngineSystem::addComponent(Ptr<IEngineSystemComponent> component)
{
    component.get().setAlreadyAddedToEngine(true);
}

void EngineSystemsManager::addComponentToEngineSystem(Ptr<IEngineSystemComponent> component)
{
    if (component and !component.get().getAlreadyAddedToEngine())
    {
        ClassId componentClassId = component.get().getClassId();
        bool added = false;
        FOR_LIST_COND(itEngineSystem, mEngineSystems, !added)
        {
            IEngineSystem *sub = (*itEngineSystem);
            if (sub->isComponentClassAccepted(componentClassId))
            {
                sub->addComponent(component);
                added = true;
            }
        }
    }
}

void EngineSystemsManager::registerEngineSystem(IEngineSystem *engineSystem)
{
    if(!CONTAINS(mEngineSystems, engineSystem))
    {
        mEngineSystems.push_back(engineSystem);
    }
}

const std::list<IEngineSystem *>& EngineSystemsManager::getEngineSystems() const
{
    return mEngineSystems;
}
