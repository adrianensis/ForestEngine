#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"

NS_BEGIN(EC)
class IComponentsListener
{
public:
    virtual void onComponentAdded(const ComponentPtrBase& componentPtr) {};
    virtual void onComponentRemoved(const ComponentPtrBase& componentPtr) {};
};

#define ECManager EC::EntityComponentManager::getInstance()

class EntityComponentManager: public Core::Singleton<EntityComponentManager>
{
public:
    void init()
    {
        mECPool.init();
    }
    void terminate()
    { 
        mECPool.terminate();
    }

    template<class T> T_EXTENDS(T, Component)
    void addComponentListener(Core::WeakPtr<EC::IComponentsListener> listener)
    {
        PROFILER_CPU()
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getClassMetadata<T>();
        Core::ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentListeners.contains(id))
        {
            mComponentListeners.emplace(id, std::unordered_set<Core::WeakPtr<EC::IComponentsListener>>());
        }

        if(!mComponentListeners.at(id).contains(listener))
        {
            mComponentListeners.at(id).emplace(listener);
        }
    }

    template<class T> T_EXTENDS(T, Component)
    void removeComponentListener(Core::WeakPtr<EC::IComponentsListener> listener)
    {
        PROFILER_CPU()
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getClassMetadata<T>();
        Core::ClassId id = classMetaData.mClassDefinition.getId();
        if(mComponentListeners.contains(id))
        {
            if(mComponentListeners.at(id).contains(listener))
            {
                mComponentListeners.at(id).erase(listener);
            }
        }
    }

    template<class T> T_EXTENDS(T, Component)
    ComponentPtr<T> requestComponent()
    {
        PROFILER_CPU()
        T* component = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getComponentsPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            component = &mECPool.getComponentsPool().getElement<T>(poolPtr);
            component->onECComponentRecycle(poolPtr.mSlot);
            #ifdef ENGINE_BUILD_DEBUG
            component->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }
        ComponentPtr<T> componentPtr(component, &mECPool);
        return componentPtr;
    }

    void addComponent(const EntityPtrBase& entityPtr, const ComponentPtrBase& componentPtr)
    {
        PROFILER_CPU()

        EntityPtrBase componentOwner = EntityPtrBase(
            componentPtr->getComponentOwner().mClassId, 
            componentPtr->getComponentOwner().mSlot, 
            componentPtr->getComponentOwner().mECPool);

        CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
        CHECK_MSG(!componentOwner.isValid(), "Component is assigned to another Entity!");
        CHECK_MSG(componentOwner != entityPtr, "Component is already assigned to Entity!");

        componentPtr->setComponentOwner(ComponentOwner(entityPtr.mClassId, entityPtr.mSlot, entityPtr.mECPool));
        
        componentOwner = EntityPtrBase(
            componentPtr->getComponentOwner().mClassId, 
            componentPtr->getComponentOwner().mSlot, 
            componentPtr->getComponentOwner().mECPool);
        CHECK_MSG(componentOwner.isValid(), "invalid Entity!");

        Core::ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        if(!mEntityComponents.contains(id))
        {
            mEntityComponents.emplace(id, std::unordered_map<Core::u32, std::list<ComponentPtrBase>>());
        }

        if(!mEntityComponents.at(id).contains((slot.getSlot())))
        {
            mEntityComponents.at(id).emplace(slot.getSlot(), std::list<ComponentPtrBase>());
        }

        mEntityComponents.at(id).at(slot.getSlot()).emplace_back(componentPtr);
        
        componentPtr->onECComponentAdded();

        notifyListenersOnComponentAdded(componentPtr);
    }

    void removeComponent(const EntityPtrBase& entityPtr, ComponentPtrBase componentPtr)
    {
        PROFILER_CPU()

        EntityPtrBase componentOwner = EntityPtrBase(
            componentPtr->getComponentOwner().mClassId, 
            componentPtr->getComponentOwner().mSlot, 
            componentPtr->getComponentOwner().mECPool);

        CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
        CHECK_MSG(componentOwner.isValid(), "Component is not assigned to a Entity!");
        CHECK_MSG(componentOwner == entityPtr, "Component is assigned to another Entity!");

        bool componentFound = false;
        Core::ClassId id = entityPtr.mClassId;
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
            notifyListenersOnComponentRemoved(componentPtr);
            componentPtr->onECComponentDestroyed();
            mECPool.getComponentsPool().removeElement(componentPtr);
        }
    }

    void removeComponents(const EntityPtrBase& entityPtr)
    {
        PROFILER_CPU()

        Core::ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        auto& components = mEntityComponents.at(id).at(slot.getSlot());
        FOR_LIST(it, components)
        {
            notifyListenersOnComponentRemoved((*it));
            (*it)->onECComponentDestroyed();
            mECPool.getComponentsPool().removeElement((*it));
        }

        components.clear();
    }

    const std::list<ComponentPtrBase>& getComponents(const EntityPtrBase& entityPtr)
    {
        Core::ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        return mEntityComponents.at(id).at(slot.getSlot());
    }

	template <class T> T_EXTENDS(T, Component)
    ComponentPtr<T> getFirstComponent(const EntityPtrBase& entityPtr)
    {
        const auto& components = getComponents(entityPtr);
        ComponentPtr<T> componentToReturn;
        FOR_LIST(it, components)
        {
            ComponentPtrBase componentPtr = (*it);
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
    T& getComponent(ComponentPtrBase componentPtr) const
    {
        return mECPool.getComponentsPool().getElement<T>(componentPtr.mSlot);
    }

    template<class T> T_EXTENDS(T, Component)
    ComponentPtr<T> getComponentPtr(T* component)
    {
        return ComponentPtr<T>(component, &mECPool);
    }

    void notifyListenersOnComponentAdded(const ComponentPtrBase& componentPtr) const
    {
        Core::ClassId id = componentPtr->getComponentTypeId();
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

    void notifyListenersOnComponentRemoved(const ComponentPtrBase& componentPtr) const
    {
        Core::ClassId id = componentPtr->getComponentTypeId();
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
    EntityPtr<T> requestEntity()
    {
        T* entity = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getEntitiesPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            entity = &mECPool.getEntitiesPool().getElement<T>(poolPtr);
            entity->onECComponentRecycle(poolPtr.mSlot);
            #ifdef ENGINE_BUILD_DEBUG
            entity->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }
        EntityPtr<T> entityPtr(entity, &mECPool);
        return entityPtr;
    }

    void removeEntity(const EntityPtrBase& entityPtr)
    {
        removeComponents(entityPtr);
        mECPool.getEntitiesPool().removeElement(entityPtr);
    }

    template<class T> T_EXTENDS(T, Entity)
    EntityPtr<T> getEntityPtr(T* entity)
    {
        return EntityPtr<T>(entity, &mECPool);
    }

private:
    EntityComponentPool mECPool;
    std::unordered_map<Core::ClassId, std::unordered_set<Core::WeakPtr<EC::IComponentsListener>>> mComponentListeners;
    std::unordered_map<Core::ClassId, std::unordered_map<Core::u32, std::list<ComponentPtrBase>>> mEntityComponents;
};
REGISTER_CLASS(EntityComponentManager);
NS_END