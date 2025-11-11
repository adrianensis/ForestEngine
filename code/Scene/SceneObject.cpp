#include "Scene/SceneObject.hpp"
#include "Engine/EntityComponent/Component.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Scene/Transform.hpp"
#include "Core/Event/EventsManager.hpp"
#include "Engine/EntityComponent/EntityComponentManager.hpp"

void SceneObject::init()
{
    PROFILER_CPU()
    EC::Entity::init();
    mTransform = ECManager.requestComponent<Transform>();
    mTransform->init();
    ECManager.addComponent(ECManager.getEntityPtr(this), mTransform);
}

IMPLEMENT_SERIALIZATION(SceneObject)
{
	SERIALIZE("transform", mTransform.get())
}

IMPLEMENT_DESERIALIZATION(SceneObject)
{
	DESERIALIZE("transform", mTransform.get())
}