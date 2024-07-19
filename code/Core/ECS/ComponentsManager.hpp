#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
#include "Core/ECS/ComponentHandler.hpp"

class Component;

class ComponentsManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    template<class T> T_EXTENDS(T, Component)
    TypedComponentHandler<T> requestComponent()
    {
        PROFILER_CPU()
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentsArrays.contains(id))
        {
            mComponentsArrays.emplace(id, OwnerPtr<ComponentsArrayBase>::moveCast(OwnerPtr<ComponentsArray<T>>::newObject(smInitialComponents)));
        }

        if(mComponentsArrays.at(id)->mSlotsManager.isEmpty())
        {
            CHECK_MSG(false, "No space available for Components!");
            // mComponentsArrays.at(id).mSlotsManager.increaseSize(smInitialComponents);
            // mComponentsArrays.at(id).mComponents.resize(mComponentsArrays.at(id).mSlotsManager.getSize());
        }

        TypedComponentHandler<T> componentHandler(id, mComponentsArrays.at(id)->mSlotsManager.requestSlot(), this);
        if(componentHandler.isValid())
        {
            // mComponentsArrays.at(id)->emplace();
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
        virtual void emplace() = 0;
        SlotsManager mSlotsManager;
    };
    template <class T> T_EXTENDS(T, Component)
    class ComponentsArray : public ComponentsArrayBase
    {
    public:
        ComponentsArray(u32 reservedComponents) : ComponentsArrayBase(reservedComponents)
        {
            PROFILER_CPU()
            PROFILER_BLOCK_CPU("Components Array")
            mComponents.resize(reservedComponents);
            PROFILER_END_BLOCK()
            mSlotsManager.init(reservedComponents);
        }
        virtual Component& at(u32 index) override
        {
            return mComponents.at(index);
        }
        virtual void emplace() override
        {
            mComponents.emplace_back();
        }
        std::vector<T> mComponents;
    };

    std::unordered_map<ClassId, OwnerPtr<ComponentsArrayBase>> mComponentsArrays;

    inline static const u32 smInitialComponents = 500000;
};
REGISTER_CLASS(ComponentsManager);