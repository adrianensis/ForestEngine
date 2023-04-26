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

	mTransform = SharedPtr<Transform>::newObject();
	addComponent<Transform>(mTransform);

	mTag = "";
}

void GameObject::addComponent(SharedPtr<Component> component, ClassId classId)
{
	if (!MAP_CONTAINS(mComponentsMap, classId))
	{
		MAP_INSERT(mComponentsMap, classId, std::list<SharedPtr<Component>>());
	}

	mComponentsMap.at(classId).push_back(component);

	component->mGameObject = getPtrToThis();
	component->onComponentAdded();

	ADD_COMPONENT_TO_ENGINE_SYSTEM(Ptr<EngineSystemComponent>::cast(component));
}

void GameObject::removeComponent(Ptr<Component> component, ClassId classId)
{
	if (MAP_CONTAINS(mComponentsMap, classId) and !component->getIsPendingToBeDestroyed() and !component->getIsDestroyed())
	{
		component->destroy();

		std::list<SharedPtr<Component>>& list = mComponentsMap.at(classId);
		list.remove(component);
	}
}

void GameObject::setIsActive(bool isActive)
{
	mIsActive = mIsDestroyed || mIsPendingToBeDestroyed ? false : isActive;

	FOR_MAP(it, mComponentsMap)
	{
		FOR_LIST(itComponent, it->second)
		{
			(*itComponent)->setIsActive(isActive);
		}
	}
}

void GameObject::destroy()
{
	mIsPendingToBeDestroyed = true;
	mIsActive = false;

	EventOnDestroy event;
	SEND_EVENT(this, this, event);

	onDestroy();

	FOR_MAP(it, mComponentsMap)
	{
		auto list = it->second;

		FOR_LIST(itComponent, list)
		{
			(*itComponent)->mGameObject.invalidate();
			(*itComponent)->destroy();
		}

		list.clear();
	}

	mComponentsMap.clear();

}

IMPLEMENT_SERIALIZATION(GameObject)
{
	SERIALIZE("class", getClassName())

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

std::list<Ptr<Component>> GameObject::getComponents(ClassId classId) const
{
	std::list<Ptr<Component>> result;
	const std::list<SharedPtr<Component>>& components = getComponentsNoCopy(classId);

	FOR_LIST(it, components)
	{
		result.push_back(*it);
	}

	return result;
}

const std::list<SharedPtr<Component>>&  GameObject::getComponentsNoCopy(ClassId classId) const
{
	const std::list<SharedPtr<Component>>* components = nullptr;

	if (MAP_CONTAINS(mComponentsMap, classId))
	{
		components = &mComponentsMap.at(classId);
	}

	if (!components || components->empty())
	{
		components = &smEmptyList;
	}

	return *components;
}

Ptr<Component> GameObject::getFirstComponent(ClassId classId) const
{
	Ptr<Component>component;
	const std::list<SharedPtr<Component>>& components = getComponentsNoCopy(classId);

	if (!components.empty())
	{
		component = components.front();
	}

	return component;
}
