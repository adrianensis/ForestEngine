#ifndef GAMEOBJECT_HPP
#define GAMEOBJECT_HPP

#include "Core/Module.hpp"
#include "Scene/Component.hpp"


class Transform;
class Scene;

class EventOnDestroy: public Event { GENERATE_METADATA(EventOnDestroy) };

class GameObject: public ObjectBase
{
    GENERATE_METADATA(GameObject)
	DECLARE_SERIALIZATION()
	
public:
    GameObject();
    ~GameObject() override;


    virtual void init();
    void addComponent(OwnerPtr<Component> component, ClassId classId);
    void removeComponent(Ptr<Component> component, ClassId classId);

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

    void setIsActive(bool isActive);
    void destroy();

private:
    const std::list<OwnerPtr<Component>>& getComponents(ClassId classId) const;
    Ptr<Component> getFirstComponent(ClassId classId) const;

private:
	inline static std::list<OwnerPtr<Component>> smEmptyList;

	std::map<ClassId, std::list<OwnerPtr<Component>>> mComponentsMap;
	bool mIsActive = false;

	Scene* mScene = nullptr;
	bool mIsStatic = false;
	OwnerPtr<Transform> mTransform;
	String mTag;
	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
	bool mShouldPersist = false;

public:
	GET_SET(Scene)
	GET_SET(IsStatic)
	GET(Transform)
	GET_SET(Tag)
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET_SET(ShouldPersist)
};

#endif