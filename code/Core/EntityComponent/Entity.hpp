#pragma once

#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Core/EntityComponent/ComponentsManager.hpp"
#include "Core/Events/Event.hpp"

class Entity: public ISerializable, public IEventObject
{
    
	DECLARE_SERIALIZATION()
	
public:
    Entity();

    virtual void init();
	void removeComponent(ComponentPtr& componentPtr);
	void addComponent(const ComponentPtr& componentPtr);

	// template <class T> T_EXTENDS(T, Component)
	// std::list<WeakPtr<T>> getComponents() const
	// {
	// 	std::list<WeakPtr<T>> components;
	// 	FOR_LIST(it, mComponentPtrs)
	// 	// FOR_LIST(it, mComponents)
	// 	{
    //         WeakPtr<T> casted = WeakPtr<T>::cast((*it).getComponent());
    //         // WeakPtr<T> casted = WeakPtr<T>::cast((*it));
    //         if(casted)
    //         {
	// 		    components.push_back(casted);
    //         }
	// 	}

	// 	return components;
	// }

	template <class T> T_EXTENDS(T, Component)
	TComponentPtr<T> getFirstComponent() const
	{   
        TComponentPtr<T> componentToReturn;
        FOR_LIST(it, mComponentPtrs)
        {
            ComponentPtr componentPtr = (*it);
            if(componentPtr.isValid())
            {
                if(dynamic_cast<const T *>(&componentPtr.getComponent()) != nullptr)
                {
                    componentToReturn = componentPtr;
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
	std::list<ComponentPtr> mComponentPtrs;
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
