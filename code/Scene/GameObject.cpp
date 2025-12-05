#include "Scene/GameObject.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/CoreMacros.hpp"
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
    mTransform = ECManager.requestComponent<Transform>(this, [&](auto* component)
    {
        component->init();
    });
}

void GameObject::setIsActive(bool isActive)
{
	mIsActive = isActive;

    std::span<EC::Component*> components = ECManager.getComponents(this);
    FOR_RANGE(i, 0, components.size())
    {
        GameComponent* gameComp = CAST(GameComponent, components[i]);
        gameComp->setIsActive(isActive);
    }
}

void GameObject::destroy()
{
    CHECK_MSG(mIsDestroyed == false, "Object already destroyed!");
	onDestroy();
	mIsDestroyed = true;

    ECManager.removeEntity(this);
}
