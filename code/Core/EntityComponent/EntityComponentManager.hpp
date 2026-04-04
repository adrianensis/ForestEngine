#pragma once

#include "Core/Assert/Assert.hpp"
#include "Core/CoreBase.hpp"
#include "Core/CoreMacros.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/EntityComponent/EntityComponentPool.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/System/System.hpp"
#include <span>
#include <vector>

NS_BEGIN(EC)
class IComponentsListener
{
public:
    virtual void onComponentAdded(Component* componentPtr) {};
    virtual void onComponentRemoved(Component* componentPtr) {};
};

class ComponentsArray
{
public:
    ComponentsArray()
    {
        
    }

    void resize(Core::u32 size)
    {
        mComponents.resize(size);
    }

    void insert(Component* component)
    {
        Core::u32 index = 0;
        if(mEmpty)
        {
            mEmpty = false;
            index = mLastIndex;
        }
        else
        {
            CHECK_MSG((mLastIndex + 1) < mComponents.size(), "No space for more components!");

            mLastIndex++;
            index = mLastIndex;
        }

        mComponents[index] = component;
    }

    void remove(Core::u32 index)
    {
        if(mComponents[index] != nullptr)
        {
            mComponents[index] = mComponents[mLastIndex];
            mComponents[mLastIndex] = nullptr;

            mLastIndex--;
        }
    }

    void clear()
    {
        mEmpty = true;
        mLastIndex = 0;
    }

    Core::u32 size() const
    {
        return mLastIndex + 1;
    }

    Core::u32 capacity() const
    {
        return mComponents.size();
    }

    bool empty() const
    {
        return mComponents.empty();
    }

    std::vector<Component*> mComponents;
    Core::u32 mLastIndex = 0;
    bool mEmpty = true;
};

class EntityComponentManager: public System::System
{
public:
    void init()
    {
        mECPool.init(smMaxSize);
    }
    virtual void terminate() override
    {
        mECPool.terminate();
    }

    template<class T> T_EXTENDS(T, Component)
    void addComponentListener(EC::IComponentsListener* listener)
    {
        PROFILER_CPU()
        const Core::ClassMetadata& classMetaData = Core::ClassManager::getClassMetadata<T>();
        Core::ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentListeners.contains(id))
        {
            mComponentListeners.emplace(id, std::unordered_set<EC::IComponentsListener*>());
        }

        if(!mComponentListeners.at(id).contains(listener))
        {
            mComponentListeners.at(id).emplace(listener);
        }
    }

    template<class T> T_EXTENDS(T, Component)
    void removeComponentListener(EC::IComponentsListener* listener)
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

    void removeAllListeners()
    {
        mComponentListeners.clear();
    }

    template<class T> T_EXTENDS(T, Component)
    T* requestComponent(Entity *entityPtr, std::function<void(T* component)> initializer)
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

        initializer(component);

        addComponent(entityPtr, component);

        return component;
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
            mEntityComponents.emplace(id, std::vector<ComponentsArray>());
            mEntityComponents.at(id).resize(smMaxSize);
        }

        ComponentsArray& componentsArray= mEntityComponents.at(id).at(slot.getSlot());
        if(componentsArray.empty())
        {
            componentsArray.resize(smMaxComponentsPerEntity);
        }

        mEntityComponents.at(id).at(slot.getSlot()).insert(componentPtr);
        
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
        auto& components =  mEntityComponents.at(id).at(slot.getSlot());
        FOR_ARRAY(i, components.mComponents)
        {
            if(components.mComponents[i] == componentPtr)
            {
                componentFound = true;
                components.remove(i);
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
        FOR_RANGE(i, 0, components.size())
        {
            notifyListenersOnComponentRemoved(components.mComponents[i]);
            components.mComponents[i]->onECComponentDestroyed();
            mECPool.getComponentsPool().removeElement(components.mComponents[i]->getPoolElementPtr());
        }

        components.clear();
    }

    std::span<Component*> getComponents(const Entity* entityPtr) const
    {
        Core::ClassId id = entityPtr->getPoolElementPtr().mClassId;
        Core::Slot slot = entityPtr->getPoolElementPtr().mSlot;
        const ComponentsArray& componentsArray = mEntityComponents.at(id).at(slot.getSlot());

        std::span<Component*> span(const_cast<ComponentsArray&>(componentsArray).mComponents.begin(), componentsArray.size());
        return span;
    }

	template <class T> T_EXTENDS(T, Component)
    T* getFirstComponent(const Entity* entityPtr) const
    {
        std::span<Component*> components = getComponents(entityPtr);
        T* componentToReturn = nullptr;
        FOR_RANGE(i, 0, components.size())
        {
            Component* componentPtr = components[i];
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
                    if((*itListener))
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
                    if((*itListener))
                    {
                        (*itListener)->onComponentRemoved(componentPtr);
                    }
                }
            }
        }
    }
    template<class T> T_EXTENDS(T, Entity)
    T* requestEntity(std::function<void(T* entity)> initializer)
    {
        T* entity = nullptr;
        Core::PoolElementPtr poolPtr = mECPool.getEntitiesPool().requestElement<T>();
        if(poolPtr.isValid())
        {
            entity = &mECPool.getEntitiesPool().getElement<T>(poolPtr);
            entity->onECEntityRecycle(poolPtr);
            #ifdef ENGINE_BUILD_DEBUG
            entity->mDebugString = Core::ClassManager::getClassMetadataById(poolPtr.mClassId).mClassDefinition.mName.getDebugString() + std::to_string(poolPtr.mSlot.getSlot());
            #endif
        }
        else
        {
            CHECK_MSG(false, "Invalid Entity!");
        }

        initializer(entity);

        return entity;
    }

    void removeEntity(Entity* entityPtr)
    {
        removeComponents(entityPtr);
        mECPool.getEntitiesPool().removeElement(entityPtr->getPoolElementPtr());
    }
    
private:
    inline static Core::u32 smMaxSize = 100000;
    inline static Core::u32 smMaxComponentsPerEntity = 50;
    EntityComponentPool mECPool;
    std::unordered_map<Core::ClassId, std::unordered_set<EC::IComponentsListener*>> mComponentListeners;
    std::unordered_map<Core::ClassId, std::vector<ComponentsArray>> mEntityComponents;
};
REGISTER_CLASS(EntityComponentManager, System)
NS_END