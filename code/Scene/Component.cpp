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
		mIsStatic = mGameObject->getIsStatic();
	}

	return mIsStatic;
}

bool Component::isActive() const
{
	return (mIsDestroyed || mIsPendingToBeDestroyed || !mGameObject) ? false : mIsActive;
}

void Component::setIsActive(bool isActive)
{
	mIsActive = (mIsDestroyed || mIsPendingToBeDestroyed || !mGameObject) ? false : isActive;
}

void Component::finallyDestroy()
{
	mIsDestroyed = true;
	mIsPendingToBeDestroyed = false;
}

void Component::destroy()
{
	if (!(getIsDestroyed() || getIsPendingToBeDestroyed()))
	{
		mIsPendingToBeDestroyed = true;
		mIsActive = false;
		onDestroy();
	}
}

void Component::onDestroy(){

}
void Component::serialize(JSON& json) const 
{
	DO_SERIALIZE("class", getClassName())
}

void Component::deserialize(const JSON& json)  {
 
}