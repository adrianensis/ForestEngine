#include "Scene/GameObject.hpp"
#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Scene/Transform.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

GameObject::GameObject()
{
    if (mGameObjectId == 0)
    {
        mGameObjectId = smGameObjectIdCounter++;
    }
}

void GameObject::init()
{
    PROFILER_CPU()
    EC::Entity::init();
    mTransform = ECManager.requestComponent<Transform>();
    mTransform->init();
    ECManager.addComponent(ECManager.getEntityPtr(this), mTransform);
}
