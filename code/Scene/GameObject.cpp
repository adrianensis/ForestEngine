#include "Scene/GameObject.hpp"
#include "Scene/Component.hpp"
#include "Scene/Transform.hpp"


GameObject::GameObject()
{
	mIsActive = true;
	mShouldPersist = false;
}

GameObject::~GameObject() 
{
}

void GameObject::init()
{
	OwnerPtr<Transform> transform = OwnerPtr<Transform>::newObject();
	addComponent<Transform>(std::move(transform));
    mTransform = getFirstComponent<Transform>();

	mTag = "";
}

Ptr<Component> GameObject::addComponent(OwnerPtr<Component>&& component)
{
    CHECK_MSG(!component->mGameObject.isValid(), "Component is already assigned to a GameObject!");

	Ptr<Component> comp = mComponents.emplace_back(std::move(component));

	comp->mGameObject = getPtrToThis();
	comp->onComponentAdded();

	SystemsManager::getInstance().addComponentToSystem(comp);

    return comp;
}

void GameObject::removeComponent(Ptr<Component> component)
{
    CHECK_MSG(component->mGameObject.isValid(), "Component is not assigned to a GameObject!");
    CHECK_MSG(component->mGameObject == getPtrToThis(), "Component is assigned to another GameObject!");

	component->destroy();

    auto it = std::find(mComponents.begin(), mComponents.end(), component);
    if (it != mComponents.end())
    {
        mComponents.erase(it);
    }
}

void GameObject::setIsActive(bool isActive)
{
	mIsActive = mIsDestroyed || mIsPendingToBeDestroyed ? false : isActive;

	FOR_LIST(it, mComponents)
	{
		(*it)->setIsActive(isActive);
	}
}

void GameObject::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	EventOnDestroy event;
	SEND_EVENT(this, this, event);

	onDestroy();

	FOR_LIST(it, mComponents)
	{
        if(*it)
        {
            (*it)->mGameObject.invalidate();
            (*it)->destroy();
        }
	}

	mComponents.clear();
}

IMPLEMENT_SERIALIZATION(GameObject)
{
	SERIALIZE("class", getClassDefinition().mName)

	SERIALIZE("is_static", mIsStatic)
	SERIALIZE("should_persist", mShouldPersist)

	SERIALIZE("tag", mTag)

	SERIALIZE("transform", mTransform.get())

}

IMPLEMENT_DESERIALIZATION(GameObject)
{
	DESERIALIZE("is_static", mIsStatic)
	DESERIALIZE("should_persist", mShouldPersist)

	DESERIALIZE("tag", mTag)

	DESERIALIZE("transform", mTransform.get())


}