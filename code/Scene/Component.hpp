#pragma once

#include "Core/Module.hpp"

#ifdef CPP_INCLUDE
#include "Scene/Component.hpp"
#include "Scene/GameObject.hpp"
#endif

class GameObject;

class Component: public IEngineSystemComponent
{
    GENERATE_METADATA(Component)
	
private:
	bool mIsActive = false;
	mutable bool mIsStatic = false;

	GameObject* mGameObject;
	
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;

public:
	CPP Component()
	{
		mIsActive = true;
		mIsDestroyed = false;
		mIsStatic = false;
	}

	virtual void init() = 0;

	virtual void onComponentAdded()
	{

	}

	CPP bool isStatic() const
	{
		if (mGameObject)
		{
			mIsStatic = mGameObject->getIsStatic();
		}

		return mIsStatic;
	}

	CPP bool isActive() const
	{
		return (mIsDestroyed || mIsPendingToBeDestroyed || !mGameObject) ? false : mIsActive;
	}

	CPP void setIsActive(bool isActive)
	{
		mIsActive = (mIsDestroyed || mIsPendingToBeDestroyed || !mGameObject) ? false : isActive;
	}

	CPP void finallyDestroy()
	{
		mIsDestroyed = true;
		mIsPendingToBeDestroyed = false;
	}

	CPP void destroy()
	{
		if (!(getIsDestroyed() || getIsPendingToBeDestroyed()))
		{
			mIsPendingToBeDestroyed = true;
			mIsActive = false;
			onDestroy();
		}
	}

	CPP virtual void onDestroy(){}

	CPP void serialize(JSON& json) const override
	{
		DO_SERIALIZE("class", getClassName())
	}

	CPP void deserialize(const JSON& json) override { };

	GET_SET(GameObject)
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};