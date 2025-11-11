#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"

Core::ClassId Script::getComponentTypeId() const { return Core::ClassManager::getClassMetadata<Script>().mClassDefinition.getId(); }

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

void Script::onEnterCollision(GameObject *otherSceneObject)
{
}

void Script::onCollision(GameObject *otherSceneObject)
{
}

void Script::onExitCollision(GameObject *otherSceneObject)
{
}

void Script::terminate()
{
}

void Script::onDestroy()
{
    terminate();
}
