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

void GameObject::addComponent(OwnerPtr<Component>&& component, ClassId classId)
{
	if (!MAP_CONTAINS(mComponentsMap, classId))
	{
		MAP_INSERT(mComponentsMap, classId, std::list<OwnerPtr<Component>>());
	}

	Ptr<Component> comp = mComponentsMap.at(classId).emplace_back(std::move(component));

	comp->mGameObject = getPtrToThis();
	comp->onComponentAdded();

	ADD_COMPONENT_TO_ENGINE_SYSTEM(Ptr<EngineSystemComponent>::cast(comp));
}

void GameObject::removeComponent(Ptr<Component> component, ClassId classId)
{
	if (MAP_CONTAINS(mComponentsMap, classId) and !component->getIsPendingToBeDestroyed() and !component->getIsDestroyed())
	{
		component->destroy();

		std::list<OwnerPtr<Component>>& list = mComponentsMap.at(classId);
        std::remove_if(
        list.begin(), list.end(),
        [component](OwnerPtr<Component>& c) { return c == component; });
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
		auto& list = it->second;

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
	const std::list<OwnerPtr<Component>>& components = getComponentsNoCopy(classId);

	FOR_LIST(it, components)
	{
		result.push_back(*it);
	}

	return result;
}

const std::list<OwnerPtr<Component>>&  GameObject::getComponentsNoCopy(ClassId classId) const
{
	const std::list<OwnerPtr<Component>>* components = nullptr;

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
	const std::list<OwnerPtr<Component>>& components = getComponentsNoCopy(classId);

	if (!components.empty())
	{
		component = components.front();
	}

	return component;
}
