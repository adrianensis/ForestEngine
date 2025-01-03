#pragma once

#include "Core/Memory/Singleton.hpp"
#include "Core/ECS/ComponentHandler.hpp"

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
    void init();
    void terminate();

    template<class T> T_EXTENDS(T, Component)
    void addComponentListener(Ptr<IComponentsListener> listener)
    {
        PROFILER_CPU()
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentListeners.contains(id))
        {
            mComponentListeners.emplace(id, std::unordered_set<Ptr<IComponentsListener>>());
        }

        if(!mComponentListeners.at(id).contains(listener))
        {
            mComponentListeners.at(id).emplace(listener);
        }
    }

    template<class T> T_EXTENDS(T, Component)
    void removeComponentListener(Ptr<IComponentsListener> listener)
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
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentsArrays.contains(id))
        {
            mComponentsArrays.emplace(id, OwnerPtr<ComponentsArrayBase>::moveCast(OwnerPtr<ComponentsArray<T>>::newObject(smMaxComponents)));
        }

        if(mComponentsArrays.at(id)->size() == smMaxComponents)
        {
            CHECK_MSG(false, "No space available for Components!");
            // mComponentsArrays.at(id).mSlotsManager.increaseSize(smInitialComponents);
            // mComponentsArrays.at(id).mComponents.resize(mComponentsArrays.at(id).mSlotsManager.getSize());
        }

        ComponentHandler componentHandler(id, mComponentsArrays.at(id)->mSlotsManager.requestSlot(), this);
        if(componentHandler.isValid())
        {
            if(componentHandler.mSlot.getSlot() == mComponentsArrays.at(id)->size())
            {
                mComponentsArrays.at(id)->emplaceBack();
            }

            Component& comp = mComponentsArrays.at(id)->at(componentHandler.mSlot.getSlot());
            T* compT = static_cast<T*>(&comp);
            *compT = T();
            Memory::registerPointer<T>(compT);
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

        ClassId id = componentHandler.mClassId;
        if(mComponentsArrays.contains(id))
        {
            mComponentsArrays.at(id)->mSlotsManager.freeSlot(componentHandler.mSlot);
        }
        Memory::unregisterPointer(&componentHandler.getComponent());
        componentHandler.reset();
    }

    template<class T> T_EXTENDS(T, Component)
    T& getComponent(ComponentHandler componentHandler) const
    {
        u32 slot = componentHandler.mSlot.getSlot();
        return mComponentsArrays.at(componentHandler.mClassId)->at(slot);
    }

    const Component& getComponentFromSlot(ClassId classId, const Slot& slot) const
    {
        return mComponentsArrays.at(classId)->at(slot.getSlot());
    }

    Component& getComponentFromSlot(ClassId classId, const Slot& slot)
    {
        return mComponentsArrays.at(classId)->at(slot.getSlot());
    }

    ComponentHandler getComponentHanlder(ClassId id, const Component& component)
    {
        ComponentHandler componentHandler(id, component.getSlot(), this);
        return componentHandler;
    }

    void notifyListenersOnComponentAdded(const ComponentHandler& componentHandler) const;
    void notifyListenersOnComponentRemoved(const ComponentHandler& componentHandler) const;

private:
    class ComponentsArrayBase
    {
    public:
        virtual ~ComponentsArrayBase() = default;
        ComponentsArrayBase(u32 reservedComponents)
        {
            mSlotsManager.init(reservedComponents);
        }
        virtual Component& at(u32 index) = 0;
        virtual u32 size() const = 0;
        virtual void emplaceBack() = 0;
        SlotsManager mSlotsManager;
    };
    template <class T> T_EXTENDS(T, Component)
    class ComponentsArray : public ComponentsArrayBase
    {
    public:
        ComponentsArray(u32 reservedComponents) : ComponentsArrayBase(reservedComponents)
        {
            PROFILER_CPU()
            mComponents.reserve(reservedComponents);
            mSlotsManager.init(reservedComponents);
        }
        virtual Component& at(u32 index) override
        {
            return mComponents.at(index);
        }
        virtual u32 size() const override
        {
            return mComponents.size();
        }
        virtual void emplaceBack() override
        {
            mComponents.emplace_back();
        }
        std::vector<T> mComponents;
    };

    std::unordered_map<ClassId, OwnerPtr<ComponentsArrayBase>> mComponentsArrays;
    std::unordered_map<ClassId, std::unordered_set<Ptr<IComponentsListener>>> mComponentListeners;

    inline static const u32 smMaxComponents = 100000;
};
REGISTER_CLASS(ComponentsManager);