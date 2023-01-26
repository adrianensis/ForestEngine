#pragma once

#include "Core/Module.hpp"

class GameObject;

class Component: public IEngineSystemComponent
{
    GENERATE_METADATA(Component)
	DECLARE_SERIALIZATION()
	
public:
    Component();

	virtual void init() = 0;

	virtual void onComponentAdded()
	{

	}

    bool isStatic() const;
    bool isActive() const;
    void setIsActive(bool isActive);
    void finallyDestroy();
    void destroy();
    virtual void onDestroy();

private:
	bool mIsActive = false;
	mutable bool mIsStatic = false;

	bool mIsDestroyed = false;
	bool mIsPendingToBeDestroyed = false;

public:
	GameObject* mGameObject;
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};
