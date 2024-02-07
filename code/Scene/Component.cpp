#include "Scene/Component.hpp"
#include "Scene/GameObject.hpp"


Component::Component()
{
	mIsActive = true;
	mIsDestroyed = false;
}

bool Component::isStatic() const
{
	return mGameObject->mIsStatic;
}

bool Component::isActive() const
{
	return (mIsDestroyed || !mGameObject) ? false : mIsActive;
}

void Component::setIsActive(bool isActive)
{
	mIsActive = (mIsDestroyed || !mGameObject) ? false : isActive;
}

void Component::destroy()
{
	CHECK_MSG(!getIsDestroyed(), "Component already destroyed");
    mIsDestroyed = true;
    mIsActive = false;
    onDestroy();
}

void Component::onDestroy()
{

}

IMPLEMENT_SERIALIZATION(Component)
{
	SERIALIZE("class", getClassDefinition().mName)
}

IMPLEMENT_DESERIALIZATION(Component)
{
 
}