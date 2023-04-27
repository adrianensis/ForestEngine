#pragma once

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
    void addComponent(SharedPtr<Component> component, ClassId classId);
    void removeComponent(Ptr<Component> component, ClassId classId);

	template <class T>
	void addComponent(SharedPtr<T> component)
	{
		GameObject::addComponent(SharedPtr<Component>::cast(component), T::getClassIdStatic());
	}

	template <class T>
	void removeComponent(Ptr<T> component)
	{
		GameObject::removeComponent(Ptr<Component>::cast(component), T::getClassIdStatic());
	}

	template <class T>
	std::list<Ptr<T>> getComponents() const
	{
		ClassId classComponentId = T::getClassIdStatic();
		const std::list<SharedPtr<Component>>& components = getComponentsNoCopy(classComponentId);

		std::list<Ptr<T>> result;
		FOR_LIST(it, components)
		{
			result.push_back(SharedPtr<T>::cast(*it));
		}

		return result;
	}

	template <class T>
	Ptr<T> getFirstComponent() const
	{
		return Ptr<T>::cast(GameObject::getFirstComponent(T::getClassIdStatic()));
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
    std::list<Ptr<Component>> getComponents(ClassId classId) const;
    const std::list<SharedPtr<Component>>&  getComponentsNoCopy(ClassId classId) const;
    Ptr<Component> getFirstComponent(ClassId classId) const;

private:
	inline static std::list<SharedPtr<Component>> smEmptyList;

	std::unordered_map<ClassId, std::list<SharedPtr<Component>>> mComponentsMap;
	bool mIsActive = false;

	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;

public:
	Ptr<Scene> mScene;
	Ptr<Transform> mTransform;
	bool mIsStatic = false;
	std::string mTag;
	bool mShouldPersist = false;
    
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
};
