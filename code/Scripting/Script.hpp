#pragma once

#include "Scene/Component.hpp"


class GameObject;

class Script: public Component
{
    GENERATE_METADATA(Script)

public:
    ClassId getEngineSystemComponentId() const override { return getClassIdStatic(); }

    virtual void init();
    virtual void firstUpdate();
    virtual void update();
    virtual void onEnterCollision(GameObject *otherGameObject);
    virtual void onCollision(GameObject *otherGameObject);
    virtual void onExitCollision(GameObject *otherGameObject);
    virtual void terminate();
	
	void firstUpdateDone()
	{
		mFirstUpdateDone = true;
	}

private:
	bool mFirstUpdateDone = false;

public:
	GET(FirstUpdateDone)
};
