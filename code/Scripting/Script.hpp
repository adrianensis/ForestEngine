#pragma once

#include "Core/Event/Event.hpp"
#include "Scene/GameComponent.hpp"


class GameObject;

class Script: public GameComponent
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
