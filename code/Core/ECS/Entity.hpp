#pragma once

#include "Core/Minimal.hpp"
#include "Core/Object/ObjectBase.hpp"
#include "Core/ECS/ComponentHandler.hpp"
#include "Core/ECS/ComponentsManager.hpp"

class Entity: public ObjectBase
{
    
	DECLARE_SERIALIZATION()
	
public:
    Entity();

    virtual void init();

    template <class T, typename ... Args> T_EXTENDS(T, Component)
	TypedComponentHandler<T> createComponent(Args&&... args)
	{
        PROFILER_CPU()
        TypedComponentHandler<T> componentHandler = GET_SYSTEM(ComponentsManager).requestComponent<T>();
        componentHandler->init(args...);
        addComponentInternal(componentHandler);
        return componentHandler;
	}

	void removeComponent(ComponentHandler componentHandler)
	{
        PROFILER_CPU()
		Entity::removeComponentInternal(componentHandler);
	}

	// template <class T> T_EXTENDS(T, Component)
	// std::list<Ptr<T>> getComponents() const
	// {
	// 	std::list<Ptr<T>> components;
	// 	FOR_LIST(it, mComponentHandlers)
	// 	// FOR_LIST(it, mComponents)
	// 	{
    //         Ptr<T> casted = Ptr<T>::cast((*it).getComponent());
    //         // Ptr<T> casted = Ptr<T>::cast((*it));
    //         if(casted)
    //         {
	// 		    components.push_back(casted);
    //         }
	// 	}

	// 	return components;
	// }

	template <class T> T_EXTENDS(T, Component)
	TypedComponentHandler<T> getFirstComponent() const
	{   
        TypedComponentHandler<T> componentToReturn;
        FOR_LIST(it, mComponentHandlers)
        {
            ComponentHandler componentHandler = (*it);
            if(componentHandler.isValid())
            {
                if(componentHandler.getComponent().template isDerivedClass<T>())
                {
                    componentToReturn = componentHandler;
                    break;
                }
            }
        }

        return componentToReturn;
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

	virtual void onDestroy(){};
    virtual void onRecycle(Slot newSlot);
    
    void setIsActive(bool isActive);
    void destroy();

private:
    // Ptr<Component> addComponentInternal(OwnerPtr<Component>&& component);
    void addComponentInternal(ComponentHandler componentHandler);
    void removeComponentInternal(ComponentHandler componentHandler);

private:
	std::list<ComponentHandler> mComponentHandlers;
	bool mIsActive = false;

	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
    Slot mSlot;

public:
	bool mIsStatic = false;
	std::string mTag;
	bool mShouldPersist = false;
    
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET(Slot)
};
REGISTER_CLASS(Entity);
