#include "Scene/Component.hpp"
#include "Scene/GameObject.hpp"


Component::Component()
{
	mIsActive = true;
	mIsDestroyed = false;
	mIsStatic = false;
}

bool Component::isStatic() const
{
	if (mGameObject)
	{
		mIsStatic = mGameObject->mIsStatic;
	}

	return mIsStatic;
}

bool Component::isActive() const
{
	return (mIsDestroyed or mIsPendingToBeDestroyed or !mGameObject) ? false : mIsActive;
}

void Component::setIsActive(bool isActive)
{
	mIsActive = (mIsDestroyed or mIsPendingToBeDestroyed or !mGameObject) ? false : isActive;
}

void Component::finallyDestroy()
{
	mIsDestroyed = true;
	mIsPendingToBeDestroyed = false;
}

void Component::destroy()
{
	if (!(getIsDestroyed() or getIsPendingToBeDestroyed()))
	{
		mIsPendingToBeDestroyed = true;
		mIsActive = false;
		onDestroy();
	}
}

void Component::onDestroy()
{

}

IMPLEMENT_SERIALIZATION(Component)
{
	SERIALIZE("class", getClassName())
}

IMPLEMENT_DESERIALIZATION(Component)
{
 
}