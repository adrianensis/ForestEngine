#pragma once

#include "Core/Minimal.hpp"
#include "Core/ECS/System.hpp"
#include "Core/ECS/Component.hpp"
#include "Core/ECS/ComponentHandler.hpp"

class ComponentsManager;

class ComponentsManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    template<class T> T_EXTENDS(T, Component)
    TypedComponentHandler<T> requestComponent()
    {
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
            Component& comp = mComponentsArrays.at(id)->at(componentHandler.mSlot.getSlot());
            comp.onRecycle(componentHandler.mSlot);
        }
        else
        {
            CHECK_MSG(false, "Invalid Component!");
        }

        // mPtrToHandler.emplace(mComponentsArrays.at(id).mComponents[slot], componentHandler);

        return componentHandler;
    }

    void removeComponent(ComponentHandler& componentHandler)
    {
        ClassId id = componentHandler.mClassId;

        if(mComponentsArrays.contains(id))
        {
            // mPtrToHandler.erase(mComponentsArrays.at(id).mComponents[slot]);
            
            // mComponentsArrays.at(componentHandler.mClassId).mComponents[slot].invalidate();
            mComponentsArrays.at(id)->mSlotsManager.freeSlot(componentHandler.mSlot);
        }
        componentHandler.reset();
    }

    // void removeComponent(Ptr<Component> component)
    // {
    //     // NOTE: copy!
    //     ComponentHandler componentHandler = mPtrToHandler.at(component);
    //     removeComponent(componentHandler);
    // }

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

    ComponentHandler getComponentHanlder(ClassId id, Component& component)
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
        SlotsManager mSlotsManager;
    };
    template <class T> T_EXTENDS(T, Component)
    class ComponentsArray : public ComponentsArrayBase
    {
    public:
        ComponentsArray(u32 reservedComponents) : ComponentsArrayBase(reservedComponents)
        {
            mComponents.resize(reservedComponents);
            mSlotsManager.init(reservedComponents);
        }
        virtual Component& at(u32 index) override
        {
            return mComponents.at(index);
        }
        std::vector<T> mComponents;
    };

    std::unordered_map<ClassId, OwnerPtr<ComponentsArrayBase>> mComponentsArrays;
    // std::unordered_map<Ptr<Component>, ComponentHandler> mPtrToHandler;

    inline static const u32 smInitialComponents = 50000;
};
REGISTER_CLASS(ComponentsManager);