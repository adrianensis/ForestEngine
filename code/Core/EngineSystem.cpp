#include "Core/EngineSystem.hpp"

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
    if (component && !component.get().getAlreadyAddedToEngine())
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
