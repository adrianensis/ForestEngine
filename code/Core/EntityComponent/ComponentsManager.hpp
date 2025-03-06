#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/ComponentHandler.hpp"
#include "Core/Memory/Pool.hpp"

class Component;

class IComponentsListener
{
public:
    virtual void onComponentAdded(const ComponentHandler& componentHandler) {};
    virtual void onComponentRemoved(const ComponentHandler& componentHandler) {};
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
    TComponentHandler<T> requestComponent()
    {
        PROFILER_CPU()
        Slot slot = mPoolsManager.requestElement<T>();
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        ComponentHandler componentHandler(classId, slot, this);
        if(componentHandler.isValid())
        {
            Component& comp = mPoolsManager.getElementBase(classId, slot);
            T* compT = static_cast<T*>(&comp);
            compT->onRecycle(componentHandler.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Component!");
        }

        return componentHandler;
    }

    void removeComponent(ComponentHandler& componentHandler)
    {
        PROFILER_CPU()

        mPoolsManager.removeElement(componentHandler.mClassId, componentHandler.mSlot);
        componentHandler.reset();
    }

    template<class T> T_EXTENDS(T, Component)
    T& getComponent(ComponentHandler componentHandler) const
    {
        return mPoolsManager.getElement<T>(componentHandler.mSlot);
    }

    void notifyListenersOnComponentAdded(const ComponentHandler& componentHandler) const;
    void notifyListenersOnComponentRemoved(const ComponentHandler& componentHandler) const;

private:
    PoolsManager<Component> mPoolsManager;
    std::unordered_map<ClassId, std::unordered_set<WeakPtr<IComponentsListener>>> mComponentListeners;

public:
    CRGET(PoolsManager)
};
REGISTER_CLASS(ComponentsManager);