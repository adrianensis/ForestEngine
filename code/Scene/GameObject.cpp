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
	//DELETE(mTransform);
}

void GameObject::addComponent(OwnerRef<Component> component, ClassId classId)
{
	if (!MAP_CONTAINS(mComponentsMap, classId))
	{
		MAP_INSERT(mComponentsMap, classId, std::list<OwnerRef<Component>>());
	}

	mComponentsMap.at(classId).push_back(component);

	component.get().setGameObject(this);
	component.get().onComponentAdded();

	ADD_COMPONENT_TO_ENGINE_SYSTEM(Ref<IEngineSystemComponent>::Cast(component));
}

void GameObject::removeComponent(Ref<Component> component, ClassId classId)
{
	if (MAP_CONTAINS(mComponentsMap, classId) && !component.get().getIsPendingToBeDestroyed() && !component.get().getIsDestroyed())
	{
		component.get().destroy();

		std::list<OwnerRef<Component>>& list = mComponentsMap.at(classId);
		list.remove(component);
	}
}

void GameObject::init()
{
	// TRACE();

	mTransform = OwnerRef<Transform>(NEW(Transform));
	addComponent<Transform>(mTransform);

	mTag = "";
}

const std::list<OwnerRef<Component>>& GameObject::getComponents(ClassId classId) const
{
	const std::list<OwnerRef<Component>>* components = nullptr;

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

Ref<Component>GameObject::getFirstComponent(ClassId classId) const
{
	Ref<Component>component;
	const std::list<OwnerRef<Component>>& components = getComponents(classId);

	if (!components.empty())
	{
		component = components.front();
	}

	return component;
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

	//DELETE(mTransform);
}

void GameObject::serialize(JSON& json) const
{
	//json["id"] = getObjectId();
	DO_SERIALIZE("class", getClassName())

	DO_SERIALIZE("is_static", mIsStatic)
	DO_SERIALIZE("should_persist", mShouldPersist)

	DO_SERIALIZE("tag", mTag)

	DO_SERIALIZE("transform", mTransform.get())

	// FOR_MAP(it, mComponentsMap)
	// {
	// 	DO_SERIALIZE_LIST_IF("components", (it->second), [](Ref<Component> component)
	// 	{
	// 		return component.get().getClassId() != Transform::getClassIdStatic();
	// 	})
	// }
}

void GameObject::deserialize(const JSON& json)
{
	DO_DESERIALIZE("is_static", mIsStatic)
	DO_DESERIALIZE("should_persist", mShouldPersist)

	DO_DESERIALIZE("tag", mTag)

	DO_DESERIALIZE("transform", mTransform.get())

	// std::list<Ref<Component>> tmpList;
	// DO_DESERIALIZE_LIST("components", tmpList, [](const JSON& json)
	// {
	// 	Ref<Component> component = OwnerRef<Component>(INSTANCE_BY_NAME(json["class"], Component));
	// 	return component;
	// })

	// FOR_LIST(it, tmpList)
	// {
	// 	(*it).get().init();
	// 	addComponent(*it, (*it).get().getClassId());
	// }
}