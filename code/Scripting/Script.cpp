#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"

ClassId Script::getComponentTypeId() const { return Core::ClassManager::getClassMetadata<Script>().mClassDefinition.getId(); }

void Script::init()
{
	mFirstUpdateDone = false;
}

void Script::firstUpdate()
{
}

void Script::update()
{
}

void Script::onEnterCollision(SceneObject *otherSceneObject)
{
}

void Script::onCollision(SceneObject *otherSceneObject)
{
}

void Script::onExitCollision(SceneObject *otherSceneObject)
{
}

void Script::terminate()
{
}

void Script::onDestroy()
{
    terminate();
}
