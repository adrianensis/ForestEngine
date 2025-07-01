#pragma once

#include "Core/Minimal.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/Component.hpp"

class Transform;
class Scene;

class SceneObject: public EC::Entity
{
	DECLARE_SERIALIZATION()
	
public:
    void init() override;
	virtual void onAddedToScene(){};

public:
	Core::WeakPtr<Scene> mScene;
	EC::TComponentPtr<Transform> mTransform;
};
REGISTER_CLASS(SceneObject);
