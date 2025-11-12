#include "Scene/GameObject.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Scene/GameComponent.hpp"
#include "Scene/Transform.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

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

void GameObject::setIsActive(bool isActive)
{
	mIsActive = isActive;

    const auto& components = ECManager.getComponents(ECManager.getEntityPtr(this));
    FOR_LIST(it, components)
    {
        EC::ComponentPtr<GameComponent> gameComp = *it;
        gameComp->setIsActive(isActive);
    }
}

void GameObject::destroy()
{
    CHECK_MSG(mIsDestroyed == false, "Object already destroyed!");
	onDestroy();
	mIsDestroyed = true;

    ECManager.removeEntity(ECManager.getEntityPtr(this));
}
