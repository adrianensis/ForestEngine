#include "Core/EngineSystem.hpp"

void IEngineSystem::init()
{
    REGISTER_ENGINE_SYSTEM(this);
}

void IEngineSystem::addComponent(IEngineSystemComponent* component)
{
    component->setAlreadyAddedToEngine(true);
}

void EngineSystemsManager::addComponentToEngineSystem(IEngineSystemComponent* component)
{
    if (component && !component->getAlreadyAddedToEngine())
    {
        ClassId componentClassId = component->getClassId();
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
