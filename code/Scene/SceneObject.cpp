#include "Scene/SceneObject.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Scene/Transform.hpp"
#include "Core/Events/EventsManager.hpp"
#include "Core/EntityComponent/EntityComponentManager.hpp"

void SceneObject::init()
{
    PROFILER_CPU()
    Entity::init();
    mTransform = EntityComponentManager::getInstance().requestComponent<Transform>();
    mTransform->init();
    addComponent(mTransform);
}

IMPLEMENT_SERIALIZATION(SceneObject)
{
	SERIALIZE("transform", mTransform.get())
}

IMPLEMENT_DESERIALIZATION(SceneObject)
{
	DESERIALIZE("transform", mTransform.get())
}