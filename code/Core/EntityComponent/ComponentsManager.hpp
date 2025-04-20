#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/Component.hpp"
#include "Core/Memory/Pool.hpp"

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
    void terminate() { mPool.terminate(); }

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
        Slot slot = mPool.requestElement<T>();
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        ComponentPtr componentPtr(classId, slot);
        if(componentPtr.isValid())
        {
            T& comp = mPool.getElement<T>(slot);
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

        mPool.removeElement(componentPtr.mClassId, componentPtr.mSlot);
        componentPtr.reset();
    }

    template<class T> T_EXTENDS(T, Component)
    T& getComponent(ComponentPtr componentPtr) const
    {
        return mPool.getElement<T>(componentPtr.mSlot);
    }

    void notifyListenersOnComponentAdded(const ComponentPtr& componentPtr) const;
    void notifyListenersOnComponentRemoved(const ComponentPtr& componentPtr) const;

private:
    Pool<Component> mPool;
    std::unordered_map<ClassId, std::unordered_set<WeakPtr<IComponentsListener>>> mComponentListeners;

public:
    CRGET(Pool)
};
REGISTER_CLASS(ComponentsManager);