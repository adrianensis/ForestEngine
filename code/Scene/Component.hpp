#pragma once

#include "Core/Module.hpp"

class GameObject;

class Component: public SystemComponent
{
    
	DECLARE_SERIALIZATION()
	
public:
    Component();

	virtual void onComponentAdded() { }

    bool isStatic() const;
    bool isActive() const;
    void setIsActive(bool isActive);
    void destroy();
    virtual void onDestroy();

private:
	bool mIsActive = false;
	bool mIsDestroyed = false;

public:
	Ptr<GameObject> mGameObject;
	GET(IsDestroyed)
};
REGISTER_CLASS(Component);
