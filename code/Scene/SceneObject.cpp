#include "Scene/SceneObject.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Scene/Transform.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

void SceneObject::init()
{
    PROFILER_CPU()
    Entity::init();
    mTransform = EntityComponentManager::getInstance().requestComponent<Transform>();
    mTransform->init();
    EntityComponentManager::getInstance().addComponent(TEntityPtr(this), mTransform);
}

IMPLEMENT_SERIALIZATION(SceneObject)
{
	SERIALIZE("transform", mTransform.get())
}

IMPLEMENT_DESERIALIZATION(SceneObject)
{
	DESERIALIZE("transform", mTransform.get())
}