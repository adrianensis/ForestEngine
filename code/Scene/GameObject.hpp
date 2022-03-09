#pragma once

#include "Core/Module.hpp"
#include "Scene/Component.hpp"

class Transform;
class Scene;

class EventOnDestroy: public Event { GENERATE_METADATA(EventOnDestroy) };

class GameObject: public ObjectBase
{
    GENERATE_METADATA(GameObject)
	
private:

	inline static std::list<OwnerRef<Component>> smEmptyList;

	std::map<ClassId, std::list<OwnerRef<Component>>> mComponentsMap;
	bool mIsActive = false;

	Scene* mScene = nullptr;
	bool mIsStatic = false;
	OwnerRef<Transform> mTransform;
	std::string mTag;
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
	bool mShouldPersist = false;

	const std::list<OwnerRef<Component>>& getComponents(ClassId classId) const;
	Ref<Component> getFirstComponent(ClassId classId) const;

	void addComponentToEngineSystem(Component* component);

public:
	GameObject();
	~GameObject() override;

	virtual void init();

	void addComponent(OwnerRef<Component> component, ClassId classId);
	void removeComponent(Ref<Component> component, ClassId classId);

	template <class T>
	void addComponent(OwnerRef<T> component)
	{
		GameObject::addComponent(OwnerRef<Component>::Cast(component), T::getClassIdStatic());
	}

	template <class T>
	void removeComponent(Ref<T> component)
	{
		GameObject::removeComponent(Ref<Component>::Cast(component), T::getClassIdStatic());
	}

	template <class T>
	const std::list<OwnerRef<T>>& getComponents() const
	{
		return reinterpret_cast<const std::list<OwnerRef<T>>&>(GameObject::getComponents(T::getClassIdStatic()));
	}

	template <class T>
	Ref<T> getFirstComponent() const
	{
		return Ref<T>::Cast(GameObject::getFirstComponent(T::getClassIdStatic()));
	}

	bool isActive() const
	{
		return (mIsDestroyed || mIsPendingToBeDestroyed) ? false : mIsActive;
	};

	void setIsActive(bool isActive);

	void finallyDestroy()
	{
		mIsDestroyed = true;
		mIsPendingToBeDestroyed = false;
	};

	void destroy();

	virtual void onDestroy(){/*TODO: call script onDestroy here??*/};

	void serialize(JSON& json) const override;
	void deserialize(const JSON& json) override;

	GET_SET(Scene)
	GET_SET(IsStatic)
	GET(Transform)
	GET_SET(Tag)
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET_SET(ShouldPersist)
};