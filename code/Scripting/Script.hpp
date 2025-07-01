#pragma once

#include "Core/EntityComponent/Component.hpp"


class SceneObject;

class Script: public Component
{
public:
    Core::ClassId getComponentTypeId() const override;

    virtual void init();
    virtual void firstUpdate();
    virtual void update();
    virtual void onEnterCollision(SceneObject *otherSceneObject);
    virtual void onCollision(SceneObject *otherSceneObject);
    virtual void onExitCollision(SceneObject *otherSceneObject);
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
