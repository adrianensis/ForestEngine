#pragma once

// CPP_IGNORE

#include "Core/Module.hpp"
#include "Scene/Component.hpp"

class Transform;
class Scene;

class EventOnDestroy: public Event { GENERATE_METADATA(EventOnDestroy) };

class GameObject: public ObjectBase
{
    GENERATE_METADATA(GameObject)
	
private:
	std::map<ClassId, std::list<Component *> *> mComponentsMap;
	bool mIsActive = false;

	Scene* mScene = nullptr;
	bool mIsStatic = false;
	Transform* mTransform = nullptr;
	std::string mTag;
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
	bool mShouldPersist = false;

	const std::list<Component *> *getComponents(ClassId classId) const;
	Component *getFirstComponent(ClassId classId) const;

	void addComponentToEngineSystem(Component* component);

public:
	GameObject();
	~GameObject() override;

	virtual void init();

	void addComponent(Component * component, ClassId classId);
	void removeComponent(Component * component, ClassId classId);

	template <class T>
	void addComponent(T * component)
	{
		GameObject::addComponent(component, T::getClassIdStatic());
	}

	template <class T>
	void removeComponent(T * component)
	{
		GameObject::removeComponent(component, T::getClassIdStatic());
	}

	template <class T>
	const std::list<T *> *getComponents() const
	{
		return reinterpret_cast<const std::list<T *> *>(GameObject::getComponents(T::getClassIdStatic()));
	}

	template <class T>
	T *getFirstComponent() const
	{
		return dynamic_cast<T *>(GameObject::getFirstComponent(T::getClassIdStatic()));
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