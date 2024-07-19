#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/Entity.hpp"

class Transform;
class Scene;

class GameObject: public Entity
{
	DECLARE_SERIALIZATION()
	
public:
    virtual void init();
	virtual void onAddedToScene(){};

public:
	Ptr<Scene> mScene;
	TypedComponentHandler<Transform> mTransform;
};
REGISTER_CLASS(GameObject);
