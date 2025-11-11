#pragma once

#include "Core/Event/Event.hpp"
#include "Engine/EntityComponent/Component.hpp"


class GameObject;

class Script: public EC::Component, public Event::IEventObject
{
public:
    Core::ClassId getComponentTypeId() const override;

    virtual void init();
    virtual void firstUpdate();
    virtual void update();
    virtual void onEnterCollision(GameObject *otherSceneObject);
    virtual void onCollision(GameObject *otherSceneObject);
    virtual void onExitCollision(GameObject *otherSceneObject);
    virtual void terminate();
    void onDestroy() override;
	
	void firstUpdateDone()
	{
		mFirstUpdateDone = true;
	}

private:
	bool mFirstUpdateDone = false;

public:
	GET(FirstUpdateDone)
};
REGISTER_CLASS(Script);
