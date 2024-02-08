#pragma once

#include "Core/Module.hpp"
#include "Core/Events/Event.hpp"
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

    virtual void init();

	template <class T> T_EXTENDS(T, Component)
	Ptr<T> addComponent(OwnerPtr<T>&& component)
	{
		return Ptr<T>::cast(GameObject::addComponent(OwnerPtr<Component>::moveCast(component)));
	}

    template <class T, typename ... Args> T_EXTENDS(T, Component)
	Ptr<T> createComponent(Args&&... args)
	{
		OwnerPtr<T> component = OwnerPtr<T>::newObject();
        component->init(args...);
        return addComponent(std::move(component));
	}

	template <class T> T_EXTENDS(T, Component)
	void removeComponent(Ptr<T> component)
	{
		GameObject::removeComponent(Ptr<Component>::cast(component));
	}

	template <class T> T_EXTENDS(T, Component)
	std::list<Ptr<T>> getComponents() const
	{
		std::list<Ptr<T>> components;
		FOR_LIST(it, mComponents)
		{
            Ptr<T> casted = Ptr<T>::cast((*it));
            if(casted)
            {
			    components.push_back(Ptr<T>::cast(*it));
            }
		}

		return components;
	}

	template <class T> T_EXTENDS(T, Component)
	Ptr<T> getFirstComponent() const
	{   
        Ptr<T> component;
        FOR_LIST(it, mComponents)
        {
            Ptr<T> casted = Ptr<T>::cast((*it));
            if(casted)
            {
                component = casted;
                break;
            }
        }

        return component;
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
    Ptr<Component> addComponent(OwnerPtr<Component>&& component);
    void removeComponent(Ptr<Component> component);

private:
	inline static std::list<OwnerPtr<Component>> smEmptyList;

	std::list<OwnerPtr<Component>> mComponents;
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
