#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Engine/EntityComponent/EntityComponentPool.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include <string>

NS_BEGIN(EC)
class IComponentsListener
{
public:
    virtual void onComponentAdded(const ComponentPtr& componentPtr) {};
    virtual void onComponentRemoved(const ComponentPtr& componentPtr) {};
};

#define ECManager EC::EntityComponentManager::getInstance()

class EntityComponentManager: public Core::Singleton<EntityComponentManager>
{
public:
    void init()
    {
        mECPool.getEntitiesPool().init(100000);
        mECPool.getComponentsPool().init(100000);
    }
    void terminate()
    { 
        mECPool.getEntitiesPool().terminate();
        mECPool.getComponentsPool().terminate();
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
    TComponentPtr<T> requestComponent()
    {
        PROFILER_CPU()
        T* component = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getComponentsPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            component = &mECPool.getComponentsPool().getElement<T>(poolPtr);
            component->onRecycle(poolPtr.mSlot);
            #ifdef ENGINE_BUILD_DEBUG
            component->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }
        TComponentPtr<T> componentPtr(component, &mECPool);
        return componentPtr;
    }

    void addComponent(const EntityPtr& entityPtr, const ComponentPtr& componentPtr)
    {
        PROFILER_CPU()
        CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
        CHECK_MSG(!componentPtr->getOwnerEntity().isValid(), "Component is assigned to another Entity!");
        CHECK_MSG(componentPtr->getOwnerEntity() != entityPtr, "Component is already assigned to Entity!");

        componentPtr->setOwnerEntity(entityPtr);
        componentPtr->mIsStatic = entityPtr->mIsStatic;
        CHECK_MSG(componentPtr->getOwnerEntity().isValid(), "invalid Entity!");

        Core::ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        if(!mEntityComponents.contains(id))
        {
            mEntityComponents.emplace(id, std::unordered_map<Core::u32, std::list<ComponentPtr>>());
        }

        if(!mEntityComponents.at(id).contains((slot.getSlot())))
        {
            mEntityComponents.at(id).emplace(slot.getSlot(), std::list<ComponentPtr>());
        }

        mEntityComponents.at(id).at(slot.getSlot()).emplace_back(componentPtr);
        
        componentPtr->onComponentAdded();

        ECManager.notifyListenersOnComponentAdded(componentPtr);
    }

    void removeComponent(const EntityPtr& entityPtr, ComponentPtr componentPtr)
    {
        PROFILER_CPU()
        CHECK_MSG(componentPtr.isValid(), "Invalid Component!");
        CHECK_MSG(componentPtr->getOwnerEntity().isValid(), "Component is not assigned to a Entity!");
        CHECK_MSG(componentPtr->getOwnerEntity() == entityPtr, "Component is assigned to another Entity!");

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
            ECManager.notifyListenersOnComponentRemoved(componentPtr);
            componentPtr->destroy();

            mECPool.getComponentsPool().removeElement(componentPtr);
        }
    }

    void removeComponents(const EntityPtr& entityPtr)
    {
        PROFILER_CPU()

        Core::ClassId id = entityPtr.mClassId;
        Core::Slot slot = entityPtr.mSlot;
        auto& components = mEntityComponents.at(id).at(slot.getSlot());
        FOR_LIST(it, components)
        {
            ECManager.notifyListenersOnComponentRemoved((*it));
            (*it)->destroy();

            mECPool.getComponentsPool().removeElement((*it));
        }

        components.clear();
    }

    const std::list<ComponentPtr>& getComponents(const EntityPtr& entityPtr)
    {
        Core::ClassId id = entityPtr.mClassId;
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
        return mECPool.getComponentsPool().getElement<T>(componentPtr.mSlot);
    }

    template<class T> T_EXTENDS(T, Component)
    TComponentPtr<T> getComponentPtr(T* component)
    {
        return TComponentPtr<T>(component, &mECPool);
    }

    void notifyListenersOnComponentAdded(const ComponentPtr& componentPtr) const
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

    void notifyListenersOnComponentRemoved(const ComponentPtr& componentPtr) const
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
    TEntityPtr<T> requestEntity()
    {
        T* entity = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getEntitiesPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            entity = &mECPool.getEntitiesPool().getElement<T>(poolPtr);
            entity->onRecycle(poolPtr.mSlot);
            #ifdef ENGINE_BUILD_DEBUG
            entity->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }
        TEntityPtr<T> entityPtr(entity, &mECPool);
        return entityPtr;
    }

    void removeEntity(EntityPtr& entityPtr)
    {
        mECPool.getEntitiesPool().removeElement(entityPtr);
        entityPtr.reset();
    }

    template<class T> T_EXTENDS(T, Entity)
    TEntityPtr<T> getEntityPtr(T* entity)
    {
        return TEntityPtr<T>(entity, &mECPool);
    }

private:
    EntityComponentPool mECPool;
    std::unordered_map<Core::ClassId, std::unordered_set<Core::WeakPtr<EC::IComponentsListener>>> mComponentListeners;
    std::unordered_map<Core::ClassId, std::unordered_map<Core::u32, std::list<ComponentPtr>>> mEntityComponents;
};
REGISTER_CLASS(EntityComponentManager);
NS_END