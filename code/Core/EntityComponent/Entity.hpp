#pragma once

#include "Core/EntityComponent/Component.hpp"
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


class EntityPtr
{
public:

    EntityPtr() = default;
    EntityPtr(ClassId id, Slot slot)
    {
        mClassId = id;
        mSlot = slot;
    }

    EntityPtr(const EntityPtr& other): EntityPtr(other.mClassId, other.mSlot)
    {
    }

    virtual ~EntityPtr()
    {
        reset();
    }

    template<class T> T_EXTENDS(T, Entity)
    T& get() const
    {
        Entity* pointer = &getInternal();
        T* castedPointer = dynamic_cast<T*>(pointer);
        CHECK_MSG(castedPointer, "Invalid Cast!");
        return *castedPointer;
    }

    Entity& getEntity() const
    {
        return get<Entity>();
    }

    EntityPtr& operator=(const EntityPtr& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
        }
        return *this;
    }

    Entity* operator->() const { return getEntityPointer(); }
    bool isValid() const { return mClassId > 0 && mSlot.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const EntityPtr& other) const
	{
		return
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
    }

protected:
    Entity* getEntityPointer() const
    {
        return& getInternal();
    }

    Entity& getInternal() const;

public:
    Slot mSlot;
    ClassId mClassId = 0;
};

template<class T>// T_EXTENDS(T, Entity)
class TEntityPtr : public EntityPtr
{
public:
    TEntityPtr() = default;
    TEntityPtr(const T* entity)
    {
        ClassId id = ClassManager::getDynamicClassMetadata(entity).mClassDefinition.getId();
        *this = TEntityPtr(id, entity->getSlot());
    }
    TEntityPtr(ClassId id, Slot slot): EntityPtr(id, slot)
    {
        checkValid();
    }

    TEntityPtr(const EntityPtr& other): TEntityPtr(other.mClassId, other.mSlot)
    {
    }

    T& get() const
    {
        return EntityPtr::get<T>();
    }

    TEntityPtr& operator=(const EntityPtr& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;

            checkValid();
        }
        return *this;
    }

    bool operator==(const T& entity) const
	{
        TEntityPtr other(entity);
		return
            mClassId == other.mClassId &&
            mSlot.getSlot() == other.mSlot.getSlot();
	}

    bool operator==(const T* entity) const
	{
        TEntityPtr other(entity);
		return
            mClassId == other.mClassId &&
            mSlot.getSlot() == other.mSlot.getSlot();
	}

    void checkValid()
    {
        if(isValid())
        {
            Entity* pointer = &getInternal();
            T* castedPointer = dynamic_cast<T*>(pointer);
            if(!castedPointer)
            {
                reset();
            }
        }
    }
    
    T* operator->() const { return &get(); }
    operator TEntityPtr<const T>() const { return TEntityPtr<const T>(mClassId, mSlot); }
    template<class U> T_EXTENDS(T, U)
    operator TEntityPtr<U>() const { return TEntityPtr<U>(mClassId, mSlot); }
};