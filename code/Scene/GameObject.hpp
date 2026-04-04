#pragma once

#include "Engine/Core.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/Event/Event.hpp"

class Transform;
class Scene;

class GameObject: public EC::Entity, public Event::IEventObject
{
public:
	GameObject();
    virtual void init();
	virtual void onAddedToScene(){};
	bool isActive() const
	{
		return mIsDestroyed ? false : mIsActive;
	};
    void setIsActive(bool isActive);
	virtual void onDestroy(){};
    void destroy();
public:
	bool mIsStatic = false;
	Scene* mScene = nullptr;
	Transform* mTransform = nullptr;
	
private:
	bool mIsDestroyed = false;
	Core::HashedString mTag;
	bool mIsActive = true;
	
	Core::u64 mGameObjectId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static Core::u64 smGameObjectIdCounter = 1;

public:
	GET(IsDestroyed)
	GET(GameObjectId)
};
REGISTER_CLASS(GameObject);
