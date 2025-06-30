#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/Entity.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include <string>

class IComponentsListener
{
public:
    virtual void onComponentAdded(const ComponentPtr& componentPtr) {};
    virtual void onComponentRemoved(const ComponentPtr& componentPtr) {};
};

#define EC EntityComponentManager::getInstance()

class EntityComponentManager: public Core::Singleton<EntityComponentManager>
{
public:
    void init()
    {
        mEntitiesPool.init(100000);
        mComponentsPool.init(100000);
    }
    void terminate()
    { 
        mEntitiesPool.terminate();
        mComponentsPool.terminate();
    }

    template<class T> T_EXTENDS(T, Component)
    void addComponentListener(Core::WeakPtr<IComponentsListener> listener)
    {
        PROFILER_CPU()
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentListeners.contains(id))
        {
            mComponentListeners.emplace(id, std::unordered_set<Core::WeakPtr<IComponentsListener>>());
        }

        if(!mComponentListeners.at(id).contains(listener))
        {
            mComponentListeners.at(id).emplace(listener);
        }
    }

    template<class T> T_EXTENDS(T, Component)
    void removeComponentListener(Core::WeakPtr<IComponentsListener> listener)
    {
        PROFILER_CPU()
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getClassMetadata<T>();
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
        Core::PoolElementPtr poolPtr = mComponentsPool.requestElement<T>();
        ComponentPtr componentPtr = poolPtr;
        if(componentPtr.isValid())
        {
            T& comp = mComponentsPool.getElement<T>(componentPtr);
            comp.onRecycle(componentPtr.mSlot);
            #ifdef ENGINE_BUILD_DEBUG
            comp.mDebugString = Core::ClassManager::getClassMetadataById(componentPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(componentPtr.mSlot.getSlot());
            componentPtr.mDebugPointer = &comp;
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Component!");
        }

        return componentPtr;
    }

    void addComponent(const EntityPtr& entityPtr, const ComponentPtr& componentPtr)
    {
        PROFILER_CPU()
        CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
        CHECK_MSG(!componentPtr->getOwnerEntity().isValid(), "Component is assigned to another Entity!");
        CHECK_MSG(componentPtr->getOwnerEntity() != entityPtr, "Component is already assigned to Entity!");

        componentPtr->setOwnerEntity(entityPtr);
        CHECK_MSG(componentPtr->getOwnerEntity().isValid(), "invalid Entity!");

        ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        if(!mEntityComponents.contains(id))
        {
            mEntityComponents.emplace(id, std::unordered_map<u32, std::list<ComponentPtr>>());
        }

        if(!mEntityComponents.at(id).contains((slot.getSlot())))
        {
            mEntityComponents.at(id).emplace(slot.getSlot(), std::list<ComponentPtr>());
        }

        mEntityComponents.at(id).at(slot.getSlot()).emplace_back(componentPtr);
        
        componentPtr->onComponentAdded();

        EC.notifyListenersOnComponentAdded(componentPtr);
    }

    void removeComponent(const EntityPtr& entityPtr, ComponentPtr componentPtr)
    {
        PROFILER_CPU()
        CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
        CHECK_MSG(componentPtr->getOwnerEntity().isValid(), "Component is not assigned to a Entity!");
        CHECK_MSG(componentPtr->getOwnerEntity() == entityPtr, "Component is assigned to another Entity!");

        bool componentFound = false;
        ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        auto& components = mEntityComponents.at(id).at(slot.getSlot());
        FOR_LIST(it, components)
        {
            if((*it) == componentPtr)
            {
                componentFound = true;
                components.erase(it);
                break;
            }
        }

        if(componentFound)
        {
            EC.notifyListenersOnComponentRemoved(componentPtr);
            componentPtr->destroy();

            mComponentsPool.removeElement(componentPtr);
        }
    }

    void removeComponents(const EntityPtr& entityPtr)
    {
        PROFILER_CPU()

        ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        auto& components = mEntityComponents.at(id).at(slot.getSlot());
        FOR_LIST(it, components)
        {
            EC.notifyListenersOnComponentRemoved((*it));
            (*it)->destroy();

            mComponentsPool.removeElement((*it));
        }

        components.clear();
    }

    const std::list<ComponentPtr>& getComponents(const EntityPtr& entityPtr)
    {
        ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        return mEntityComponents.at(id).at(slot.getSlot());
    }

	template <class T> T_EXTENDS(T, Component)
    TComponentPtr<T> getFirstComponent(const EntityPtr& entityPtr)
    {
        const auto& components = getComponents(entityPtr);
        TComponentPtr<T> componentToReturn;
        FOR_LIST(it, components)
        {
            ComponentPtr componentPtr = (*it);
            if(componentPtr.isValid())
            {
                if(dynamic_cast<const T *>(&componentPtr.get<Component>()) != nullptr)
                {
                    componentToReturn = componentPtr;
                    break;
                }
            }
        }

        return componentToReturn;
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
        Core::PoolElementPtr poolPtr = mEntitiesPool.requestElement<T>();
        EntityPtr entityPtr = poolPtr;
        if(entityPtr.isValid())
        {
            T& entity = mEntitiesPool.getElement<T>(poolPtr);
            entity.onRecycle(entityPtr.mSlot);
            #ifdef ENGINE_BUILD_DEBUG
            entity.mDebugString = Core::ClassManager::getClassMetadataById(entityPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(entityPtr.mSlot.getSlot());
            entityPtr.mDebugPointer = &entity;
            #endif
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
    Core::Pool<Entity> mEntitiesPool;
    Core::Pool<Component> mComponentsPool;
    std::unordered_map<ClassId, std::unordered_set<Core::WeakPtr<IComponentsListener>>> mComponentListeners;
    std::unordered_map<ClassId, std::unordered_map<u32, std::list<ComponentPtr>>> mEntityComponents;

public:
    CRGET(EntitiesPool)
    CRGET(ComponentsPool)
};
REGISTER_CLASS(EntityComponentManager);