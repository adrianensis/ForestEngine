#pragma once

#include "Engine/Core.hpp"
#include "Engine/EntityComponent/Entity.hpp"
#include "Engine/EntityComponent/Component.hpp"

class Transform;
class Scene;

class GameObject: public EC::Entity, public Event::IEventObject
{
public:
	GameObject();
    void init() override;
	virtual void onAddedToScene(){};

public:
	bool mIsStatic = false;
	Core::Ptr<Scene> mScene;
	EC::ComponentPtr<Transform> mTransform;

private:
	Core::HashedString mTag;

	Core::u64 mGameObjectId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u64 smGameObjectIdCounter = 1;

public:
	GET(GameObjectId)

};
REGISTER_CLASS(GameObject);
