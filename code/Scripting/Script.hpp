#pragma once

#include "Core/Event/Event.hpp"
#include "Scene/GameComponent.hpp"


class GameObject;

class Script: public GameComponent
{
public:
    virtual void init();
    virtual void firstUpdate(Core::f32 dt);
    virtual void update(Core::f32 dt);
    virtual void onEnterCollision(GameObject *otherGameObject);
    virtual void onCollision(GameObject *otherGameObject);
    virtual void onExitCollision(GameObject *otherGameObject);
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
REGISTER_CLASS(Script, GameComponent);
