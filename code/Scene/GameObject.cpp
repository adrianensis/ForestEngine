#include "Scene/GameObject.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Scene/Transform.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"

void GameObject::init()
{
    PROFILER_CPU()
    Entity::init();
    mTransform = ComponentsManager::getInstance().requestComponent<Transform>();
    mTransform->init();
    addComponent(mTransform);
}

IMPLEMENT_SERIALIZATION(GameObject)
{
	SERIALIZE("transform", mTransform.get())
}

IMPLEMENT_DESERIALIZATION(GameObject)
{
	DESERIALIZE("transform", mTransform.get())
}