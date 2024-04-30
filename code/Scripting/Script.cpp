#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"

ClassId Script::getSystemComponentId() const { return ClassManager::getClassMetadata<Script>().mClassDefinition.getId(); }

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

void Script::onEnterCollision(GameObject *otherGameObject)
{
}

void Script::onCollision(GameObject *otherGameObject)
{
}

void Script::onExitCollision(GameObject *otherGameObject)
{
}

void Script::terminate()
{
}

void Script::onDestroy()
{
    terminate();
}
