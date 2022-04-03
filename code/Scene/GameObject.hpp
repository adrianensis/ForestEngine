#pragma once

#include "Core/Module.hpp"
#include "Scene/Component.hpp"

#ifdef CPP_INCLUDE
#include "Scene/GameObject.hpp"
#include "Scene/Component.hpp"
#include "Scene/Transform.hpp"
#endif

class Transform;
class Scene;

class EventOnDestroy: public Event { GENERATE_METADATA(EventOnDestroy) };

class GameObject: public ObjectBase
{
    GENERATE_METADATA(GameObject)
	
private:

	inline static std::list<OwnerPtr<Component>> smEmptyList;

	std::map<ClassId, std::list<OwnerPtr<Component>>> mComponentsMap;
	bool mIsActive = false;

	Scene* mScene = nullptr;
	bool mIsStatic = false;
	OwnerPtr<Transform> mTransform;
	std::string mTag;
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
	bool mShouldPersist = false;

	CPP const std::list<OwnerPtr<Component>>& getComponents(ClassId classId) const
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

	CPP Ptr<Component> getFirstComponent(ClassId classId) const
	{
		Ptr<Component>component;
		const std::list<OwnerPtr<Component>>& components = getComponents(classId);

		if (!components.empty())
		{
			component = components.front();
		}

		return component;
	}

public:
	CPP GameObject()
	{
		mTransform = nullptr;
		mScene = nullptr;
		mIsActive = true;
		mShouldPersist = false;
	}

	CPP ~GameObject() override
	{
		//DELETE(mTransform);
	}


	CPP virtual void init()
	{
		// TRACE();

		mTransform = OwnerPtr<Transform>(NEW(Transform));
		addComponent<Transform>(mTransform);

		mTag = "";
	}

	CPP void addComponent(OwnerPtr<Component> component, ClassId classId)
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

	CPP void removeComponent(Ptr<Component> component, ClassId classId)
	{
		if (MAP_CONTAINS(mComponentsMap, classId) && !component.get().getIsPendingToBeDestroyed() && !component.get().getIsDestroyed())
		{
			component.get().destroy();

			std::list<OwnerPtr<Component>>& list = mComponentsMap.at(classId);
			list.remove(component);
		}
	}


	template <class T>
	void addComponent(OwnerPtr<T> component)
	{
		GameObject::addComponent(OwnerPtr<Component>::Cast(component), T::getClassIdStatic());
	}

	template <class T>
	void removeComponent(Ptr<T> component)
	{
		GameObject::removeComponent(Ptr<Component>::Cast(component), T::getClassIdStatic());
	}

	template <class T>
	const std::list<OwnerPtr<T>>& getComponents() const
	{
		return reinterpret_cast<const std::list<OwnerPtr<T>>&>(GameObject::getComponents(T::getClassIdStatic()));
	}

	template <class T>
	Ptr<T> getFirstComponent() const
	{
		return Ptr<T>::Cast(GameObject::getFirstComponent(T::getClassIdStatic()));
	}

	bool isActive() const
	{
		return (mIsDestroyed || mIsPendingToBeDestroyed) ? false : mIsActive;
	};

	void finallyDestroy()
	{
		mIsDestroyed = true;
		mIsPendingToBeDestroyed = false;
	};

	virtual void onDestroy(){/*TODO: call script onDestroy here??*/};

	CPP void setIsActive(bool isActive)
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

	CPP void destroy()
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

	CPP void serialize(JSON& json) const override
	{
		//json["id"] = getObjectId();
		DO_SERIALIZE("class", getClassName())

		DO_SERIALIZE("is_static", mIsStatic)
		DO_SERIALIZE("should_persist", mShouldPersist)

		DO_SERIALIZE("tag", mTag)

		DO_SERIALIZE("transform", mTransform.get())

		// FOR_MAP(it, mComponentsMap)
		// {
		// 	DO_SERIALIZE_LIST_IF("components", (it->second), [](Ptr<Component> component)
		// 	{
		// 		return component.get().getClassId() != Transform::getClassIdStatic();
		// 	})
		// }
	}

	CPP void deserialize(const JSON& json) override
	{
		DO_DESERIALIZE("is_static", mIsStatic)
		DO_DESERIALIZE("should_persist", mShouldPersist)

		DO_DESERIALIZE("tag", mTag)

		DO_DESERIALIZE("transform", mTransform.get())

		// std::list<Ptr<Component>> tmpList;
		// DO_DESERIALIZE_LIST("components", tmpList, [](const JSON& json)
		// {
		// 	Ptr<Component> component = OwnerPtr<Component>(INSTANCE_BY_NAME(json["class"], Component));
		// 	return component;
		// })

		// FOR_LIST(it, tmpList)
		// {
		// 	(*it).get().init();
		// 	addComponent(*it, (*it).get().getClassId());
		// }
	}

	GET_SET(Scene)
	GET_SET(IsStatic)
	GET(Transform)
	GET_SET(Tag)
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET_SET(ShouldPersist)
};