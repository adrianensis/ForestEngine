#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/ComponentPtr.hpp"
#include "Core/Memory/Pool.hpp"

class Component;

class IComponentsListener
{
public:
    virtual void onComponentAdded(const ComponentPtr& componentPtr) {};
    virtual void onComponentRemoved(const ComponentPtr& componentPtr) {};
};

class ComponentsManager: public Singleton<ComponentsManager>
{
public:
    void init() {}
    void terminate() { mPoolsManager.terminate(); }

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
        Slot slot = mPoolsManager.requestElement<T>();
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        ComponentPtr componentPtr(classId, slot, this);
        if(componentPtr.isValid())
        {
            T& comp = mPoolsManager.getElement<T>(slot);
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

        mPoolsManager.removeElement(componentPtr.mClassId, componentPtr.mSlot);
        componentPtr.reset();
    }

    template<class T> T_EXTENDS(T, Component)
    T& getComponent(ComponentPtr componentPtr) const
    {
        return mPoolsManager.getElement<T>(componentPtr.mSlot);
    }

    void notifyListenersOnComponentAdded(const ComponentPtr& componentPtr) const;
    void notifyListenersOnComponentRemoved(const ComponentPtr& componentPtr) const;

private:
    PoolsManager<Component> mPoolsManager;
    std::unordered_map<ClassId, std::unordered_set<WeakPtr<IComponentsListener>>> mComponentListeners;

public:
    CRGET(PoolsManager)
};
REGISTER_CLASS(ComponentsManager);