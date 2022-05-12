#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "Core/Module.hpp"

class GameObject;

class Component: public IEngineSystemComponent
{
    GENERATE_METADATA(Component)
	
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
    void serialize(JSON& json) const override;
    void deserialize(const JSON& json) override;

private:
	bool mIsActive = false;
	mutable bool mIsStatic = false;

	GameObject* mGameObject;
	
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;

public:
	GET_SET(GameObject)
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};

#endif