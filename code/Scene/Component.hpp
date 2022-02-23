#pragma once

#include "Core/Module.hpp"
class GameObject;

class Component: public ObjectBase
{
    GENERATE_METADATA(Component)
	
private:
	bool mIsActive = false;
	bool mIsStatic = false;

	GameObject* mGameObject;
	bool mAlreadyAddedToEngine = false;
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;

public:
	Component();

	virtual void init() = 0;

	virtual void onComponentAdded() { };

	bool isStatic();

	bool isActive() const
	{
		return (mIsDestroyed || mIsPendingToBeDestroyed || !mGameObject) ? false : mIsActive;
	};

	void setIsActive(bool isActive)
	{
		mIsActive = (mIsDestroyed || mIsPendingToBeDestroyed || !mGameObject) ? false : isActive;
	};

	void finallyDestroy()
	{
		mIsDestroyed = true;
		mIsPendingToBeDestroyed = false;
	};

	void destroy();

	virtual void onDestroy(){};

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

	GET_SET(GameObject)
	GET_SET(AlreadyAddedToEngine)
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};