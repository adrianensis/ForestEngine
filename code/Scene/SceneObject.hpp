#pragma once

#include "Core/Minimal.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/Component.hpp"

class Transform;
class Scene;

class SceneObject: public Entity
{
	DECLARE_SERIALIZATION()
	
public:
    void init() override;
	virtual void onAddedToScene(){};

public:
	Core::WeakPtr<Scene> mScene;
	TComponentPtr<Transform> mTransform;
};
REGISTER_CLASS(SceneObject);
