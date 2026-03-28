#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"

void Script::init()
{
	mFirstUpdateDone = false;
}

void Script::firstUpdate(Core::f32 dt)
{
}

void Script::update(Core::f32 dt)
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
