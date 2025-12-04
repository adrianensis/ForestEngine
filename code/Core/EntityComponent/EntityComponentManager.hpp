#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"

NS_BEGIN(EC)
class IComponentsListener
{
public:
    virtual void onComponentAdded(Component* componentPtr) {};
    virtual void onComponentRemoved(Component* componentPtr) {};
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
    T* requestComponent()
    {
        PROFILER_CPU()
        T* component = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getComponentsPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            component = &mECPool.getComponentsPool().getElement<T>(poolPtr);
            component->onECComponentRecycle(poolPtr);
            #ifdef ENGINE_BUILD_DEBUG
            component->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }
        T* componentPtr(component);
        return componentPtr;
    }

    void addComponent(Entity* entityPtr, Component* componentPtr)
    {
        PROFILER_CPU()

        CHECK_MSG(componentPtr, "Invalid Component!");
        CHECK_MSG(!componentPtr->getComponentOwner(), "Component is assigned to another Entity!");
        CHECK_MSG(componentPtr->getComponentOwner() != entityPtr, "Component is already assigned to Entity!");

        componentPtr->setComponentOwner(entityPtr);
        
        CHECK_MSG(componentPtr->getComponentOwner(), "invalid Entity!");

        Core::ClassId id = entityPtr->getPoolElementPtr().mClassId;
        Core::Slot slot = entityPtr->getPoolElementPtr().mSlot;
        if(!mEntityComponents.contains(id))
        {
            mEntityComponents.emplace(id, std::unordered_map<Core::u32, std::list<Component*>>());
        }

        if(!mEntityComponents.at(id).contains((slot.getSlot())))
        {
            mEntityComponents.at(id).emplace(slot.getSlot(), std::list<Component*>());
        }

        mEntityComponents.at(id).at(slot.getSlot()).emplace_back(componentPtr);
        
        componentPtr->onECComponentAdded();

        notifyListenersOnComponentAdded(componentPtr);
    }

    void removeComponent(Entity* entityPtr, Component* componentPtr)
    {
        PROFILER_CPU()
        CHECK_MSG(componentPtr, "Invalid Component!");
        CHECK_MSG(componentPtr->getComponentOwner(), "Component is not assigned to a Entity!");
        CHECK_MSG(componentPtr->getComponentOwner() == entityPtr, "Component is assigned to another Entity!");

        bool componentFound = false;
        Core::ClassId id = entityPtr->getPoolElementPtr().mClassId;
        Core::Slot slot = entityPtr->getPoolElementPtr().mSlot;
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
            mECPool.getComponentsPool().removeElement(componentPtr->getPoolElementPtr());
        }
    }

    void removeComponents(Entity* entityPtr)
    {
        PROFILER_CPU()

        Core::ClassId id = entityPtr->getPoolElementPtr().mClassId;
        Core::Slot slot = entityPtr->getPoolElementPtr().mSlot;
        auto& components = mEntityComponents.at(id).at(slot.getSlot());
        FOR_LIST(it, components)
        {
            notifyListenersOnComponentRemoved((*it));
            (*it)->onECComponentDestroyed();
            mECPool.getComponentsPool().removeElement((*it)->getPoolElementPtr());
        }

        components.clear();
    }

    const std::list<Component*>& getComponents(const Entity* entityPtr)
    {
        Core::ClassId id = entityPtr->getPoolElementPtr().mClassId;
        Core::Slot slot = entityPtr->getPoolElementPtr().mSlot;
        return mEntityComponents.at(id).at(slot.getSlot());
    }

	template <class T> T_EXTENDS(T, Component)
    T* getFirstComponent(const Entity* entityPtr)
    {
        const auto& components = getComponents(entityPtr);
        T* componentToReturn;
        FOR_LIST(it, components)
        {
            Component* componentPtr = (*it);
            if(componentPtr)
            {
                componentToReturn = dynamic_cast<T *>(componentPtr);
                if(componentToReturn != nullptr)
                {
                    break;
                }
            }
        }

        return componentToReturn;
    }

    template<class T> T_EXTENDS(T, Component)
    T* getComponent(Component* componentPtr) const
    {
        if(componentPtr)
        {
            T* comp = &mECPool.getComponentsPool().getElement<T>(componentPtr);
            return comp;
        }

        return nullptr;
    }

    void notifyListenersOnComponentAdded(Component* componentPtr) const
    {
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getDynamicClassMetadata(componentPtr);
        FOR_MAP(it, mComponentListeners)
        {
            bool classFound = classMetaData.mClassDefinition.isA(it->first);
            if(classFound)
            {
                FOR_LIST(itListener, it->second)
                {
                    if((*itListener).isValid())
                    {
                        (*itListener)->onComponentAdded(componentPtr);
                    }
                }
            }
        }
    }

    void notifyListenersOnComponentRemoved(Component* componentPtr) const
    {
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getDynamicClassMetadata(componentPtr);
        FOR_MAP(it, mComponentListeners)
        {
            bool classFound = classMetaData.mClassDefinition.isA(it->first);
            if(classFound)
            {
                FOR_LIST(itListener, it->second)
                {
                    if((*itListener).isValid())
                    {
                        (*itListener)->onComponentRemoved(componentPtr);
                    }
                }
            }
        }
    }
    template<class T> T_EXTENDS(T, Entity)
    T* requestEntity()
    {
        T* entity = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getEntitiesPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            entity = &mECPool.getEntitiesPool().getElement<T>(poolPtr);
            entity->onECComponentRecycle(poolPtr);
            #ifdef ENGINE_BUILD_DEBUG
            entity->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }
        return entity;
    }

    void removeEntity(Entity* entityPtr)
    {
        removeComponents(entityPtr);
        mECPool.getEntitiesPool().removeElement(entityPtr->getPoolElementPtr());
    }
    
private:
    EntityComponentPool mECPool;
    std::unordered_map<Core::ClassId, std::unordered_set<Core::WeakPtr<EC::IComponentsListener>>> mComponentListeners;
    std::unordered_map<Core::ClassId, std::unordered_map<Core::u32, std::list<Component*>>> mEntityComponents;
};
NS_END