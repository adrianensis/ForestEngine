#pragma once

#include "Core/Module.hpp"

class GameObject;

class Component: public EngineSystemComponent
{
    GENERATE_METADATA(Component)
	DECLARE_SERIALIZATION()
	
public:
    Component();

	virtual void init() {};

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
	Ptr<GameObject> mGameObject;
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};

class ComponentData
{
public:
    virtual ~ComponentData() = default;
};

template<class T> T_EXTENDS(T, ComponentData)
class ComponentWithData: public Component
{
public:
	virtual void init(T& data)
    {
        mComponentData = data;
    };

protected:
    T mComponentData;

public:
    CRGET(ComponentData)
};