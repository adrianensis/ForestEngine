#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/Memory/Pool.hpp"

class IComponentsListener
{
public:
    virtual void onComponentAdded(const ComponentPtr& componentPtr) {};
    virtual void onComponentRemoved(const ComponentPtr& componentPtr) {};
};

class EntityComponentManager: public Singleton<EntityComponentManager>
{
public:
    void init() {}
    void terminate()
    { 
        mEntitiesPool.terminate();
        mComponentsPool.terminate();
    }

    template<class T> T_EXTENDS(T, Component)
    void addComponentListener(WeakPtr<IComponentsListener> listener)
    {
        PROFILER_CPU()
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentListeners.contains(id))
        {
            mComponentListeners.emplace(id, std::unordered_set<WeakPtr<IComponentsListener>>());
        }

        if(!mComponentListeners.at(id).contains(listener))
        {
            mComponentListeners.at(id).emplace(listener);
        }
    }

    template<class T> T_EXTENDS(T, Component)
    void removeComponentListener(WeakPtr<IComponentsListener> listener)
    {
        PROFILER_CPU()
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(mComponentListeners.contains(id))
        {
            if(mComponentListeners.at(id).contains(listener))
            {
                mComponentListeners.at(id).erase(listener);
            }
        }
    }

    template<class T> T_EXTENDS(T, Component)
    TComponentPtr<T> requestComponent()
    {
        PROFILER_CPU()
        PoolElementPtr poolPtr = mComponentsPool.requestElement<T>();
        ComponentPtr componentPtr = poolPtr;
        if(componentPtr.isValid())
        {
            T& comp = mComponentsPool.getElement<T>(componentPtr);
            comp.onRecycle(componentPtr.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Component!");
        }

        return componentPtr;
    }

    void removeComponent(ComponentPtr& componentPtr)
    {
        PROFILER_CPU()

        mComponentsPool.removeElement(componentPtr);
        componentPtr.reset();
    }

    template<class T> T_EXTENDS(T, Component)
    T& getComponent(ComponentPtr componentPtr) const
    {
        return mComponentsPool.getElement<T>(componentPtr.mSlot);
    }

    void notifyListenersOnComponentAdded(const ComponentPtr& componentPtr) const
    {
        ClassId id = componentPtr->getComponentTypeId();
        if(mComponentListeners.contains(id))
        {
            FOR_LIST(it, mComponentListeners.at(id))
            {
                if((*it).isValid())
                {
                    (*it)->onComponentAdded(componentPtr);
                }
            }
        }
    }

    void notifyListenersOnComponentRemoved(const ComponentPtr& componentPtr) const
    {
        ClassId id = componentPtr->getComponentTypeId();
        if(mComponentListeners.contains(id))
        {
            FOR_LIST(it, mComponentListeners.at(id))
            {
                if((*it).isValid())
                {
                    (*it)->onComponentRemoved(componentPtr);
                }
            }
        }
    }
    template<class T> T_EXTENDS(T, Entity)
    TEntityPtr<T> requestEntity()
    {
        PoolElementPtr poolPtr = mEntitiesPool.requestElement<T>();
        EntityPtr entityPtr = poolPtr;
        if(entityPtr.isValid())
        {
            T& entity = mEntitiesPool.getElement<T>(poolPtr);
            entity.onRecycle(entityPtr.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }

        return entityPtr;
    }

    void removeEntity(EntityPtr& entityPtr)
    {
        mEntitiesPool.removeElement(entityPtr);
        entityPtr.reset();
    }

private:
    Pool<Entity> mEntitiesPool;
    Pool<Component> mComponentsPool;
    std::unordered_map<ClassId, std::unordered_set<WeakPtr<IComponentsListener>>> mComponentListeners;

public:
    CRGET(EntitiesPool)
    CRGET(ComponentsPool)
};
REGISTER_CLASS(EntityComponentManager);