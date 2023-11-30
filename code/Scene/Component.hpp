#pragma once

#include "Core/Module.hpp"

class GameObject;

class Component: public SystemComponent
{
    GENERATE_METADATA(Component)
	DECLARE_SERIALIZATION()
	
public:
    Component();

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

	bool mIsDestroyed = false;
	bool mIsPendingToBeDestroyed = false;

public:
	Ptr<GameObject> mGameObject;
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};