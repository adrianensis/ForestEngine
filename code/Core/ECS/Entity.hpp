#pragma once

#include "Core/ECS/ComponentHandler.hpp"
#include "Core/ECS/ComponentsManager.hpp"
#include "Core/Events/Event.hpp"

class Entity: public ISerializable, public IEventObject
{
    
	DECLARE_SERIALIZATION()
	
public:
    Entity();

    virtual void init();

    template <class T, typename ... Args> T_EXTENDS(T, Component)
	TComponentHandler<T> createComponent(Args&&... args)
	{
        PROFILER_CPU()
        TComponentHandler<T> componentHandler = ComponentsManager::getInstance().requestComponent<T>();
        setComponentOwner(componentHandler);
        componentHandler->init(args...);
        addComponentInternal(componentHandler);
        return componentHandler;
	}

	void removeComponent(ComponentHandler componentHandler);

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
	TComponentHandler<T> getFirstComponent() const
	{   
        TComponentHandler<T> componentToReturn;
        FOR_LIST(it, mComponentHandlers)
        {
            ComponentHandler componentHandler = (*it);
            if(componentHandler.isValid())
            {
                if(dynamic_cast<const T *>(&componentHandler.getComponent()) != nullptr)
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
    void setComponentOwner(ComponentHandler componentHandler);
    void addComponentInternal(ComponentHandler componentHandler);
    void removeComponentInternal(ComponentHandler componentHandler);

private:
	std::list<ComponentHandler> mComponentHandlers;
	bool mIsActive = true;

	bool mIsPendingToBeDestroyed = false;
	bool mIsDestroyed = false;
    Slot mSlot;

	u64 mEntityId = 0;
    // Important: starts by 1, 0 is reserved for null
	inline static u64 smEntityIdCounter = 1;

public:
	bool mIsStatic = false;
	HashedString mTag;
	bool mShouldPersist = false;
    
	GET(IsPendingToBeDestroyed)
	GET(IsDestroyed)
	GET(Slot)
	GET(EntityId)
};
REGISTER_CLASS(Entity);
