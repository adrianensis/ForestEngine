#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"


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
