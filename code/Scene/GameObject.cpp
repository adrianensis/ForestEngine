#include "Scene/GameObject.hpp"
#include "Scene/Component.hpp"
#include "Scene/Transform.hpp"


GameObject::GameObject()
{
	mTransform = nullptr;
	mScene = nullptr;
	mIsActive = true;
	mShouldPersist = false;
}

GameObject::~GameObject() 
{
}

void GameObject::init()
{

	mTransform = OwnerPtr<Transform>(NEW(Transform));
	addComponent<Transform>(mTransform);

	mTag = "";
}

void GameObject::addComponent(OwnerPtr<Component> component, ClassId classId)
{
	if (!MAP_CONTAINS(mComponentsMap, classId))
	{
		MAP_INSERT(mComponentsMap, classId, std::list<OwnerPtr<Component>>());
	}

	mComponentsMap.at(classId).push_back(component);

	component.get().setGameObject(this);
	component.get().onComponentAdded();

	ADD_COMPONENT_TO_ENGINE_SYSTEM(Ptr<IEngineSystemComponent>::Cast(component));
}

void GameObject::removeComponent(Ptr<Component> component, ClassId classId)
{
	if (MAP_CONTAINS(mComponentsMap, classId) && !component.get().getIsPendingToBeDestroyed() && !component.get().getIsDestroyed())
	{
		component.get().destroy();

		std::list<OwnerPtr<Component>>& list = mComponentsMap.at(classId);
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
			(*itComponent).get().setIsActive(isActive);
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
			(*itComponent).get().setGameObject(nullptr);
			(*itComponent).get().destroy();
		}

		list.clear();
	}

	mComponentsMap.clear();

}

void GameObject::serialize(JSON& json) const 
{
	DO_SERIALIZE("class", getClassName())

	DO_SERIALIZE("is_static", mIsStatic)
	DO_SERIALIZE("should_persist", mShouldPersist)

	DO_SERIALIZE("tag", mTag)

	DO_SERIALIZE("transform", mTransform.get())

}

void GameObject::deserialize(const JSON& json) 
{
	DO_DESERIALIZE("is_static", mIsStatic)
	DO_DESERIALIZE("should_persist", mShouldPersist)

	DO_DESERIALIZE("tag", mTag)

	DO_DESERIALIZE("transform", mTransform.get())


}

const std::list<OwnerPtr<Component>>& GameObject::getComponents(ClassId classId) const
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
	const std::list<OwnerPtr<Component>>& components = getComponents(classId);

	if (!components.empty())
	{
		component = components.front();
	}

	return component;
}
