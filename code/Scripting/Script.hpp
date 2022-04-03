#pragma once

#include "Scene/Component.hpp"

#ifdef CPP_INCLUDE
#include "Scripting/Script.hpp"
#include "Scene/Module.hpp"
#endif

class GameObject;

class Script: public Component
{
    GENERATE_METADATA(Script)
private:
	bool mFirstUpdateDone = false;

public:
	CPP virtual void init()
	{
		mFirstUpdateDone = false;
	}

	CPP virtual void firstUpdate()
	{
	}

	CPP virtual void update()
	{
	}

	CPP virtual void onEnterCollision(GameObject *otherGameObject)
	{
	}

	CPP virtual void onCollision(GameObject *otherGameObject)
	{
	}

	CPP virtual void onExitCollision(GameObject *otherGameObject)
	{
	}

	CPP virtual void terminate()
	{
	}
	
	void firstUpdateDone()
	{
		mFirstUpdateDone = true;
	}

	GET(FirstUpdateDone)
};