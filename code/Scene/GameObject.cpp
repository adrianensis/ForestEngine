#include "Scene/GameObject.hpp"
#include "Core/ECS/Component.hpp"
#include "Scene/Transform.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Core/ECS/ComponentsManager.hpp"

void GameObject::init()
{
    Entity::init();
    mTransform = createComponent<Transform>();
}

IMPLEMENT_SERIALIZATION(GameObject)
{
	SERIALIZE("transform", mTransform.get())
}

IMPLEMENT_DESERIALIZATION(GameObject)
{
	DESERIALIZE("transform", mTransform.get())
}