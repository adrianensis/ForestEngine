#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Scene/Component.hpp"

class ComponentsManager;

class ComponentHandler
{
public:
    ComponentHandler() = default;
    ComponentHandler(ClassId id, Slot slot, ComponentsManager* componentsManager)
    {
        mClassId = id;
        mSlot = slot;
        mComponentsManager = componentsManager;
        CHECK_MSG(isValid(), "Invalid handler!");
    }

    ComponentHandler(ClassId id, Slot slot, const ComponentsManager* componentsManager): ComponentHandler(id, slot, const_cast<ComponentsManager*>(componentsManager))
    {
    }

    ~ComponentHandler()
    {
        reset();
    }

    template<class T> T_EXTENDS(T, Component)
    Ptr<T> get() const
    {
        return Ptr<T>::cast(getComponent());
    }

    Ptr<Component> operator->() const { return getComponent(); }

    Ptr<Component> getComponent() const;
    bool isValid() const { return mComponentsManager && mClassId > 0 && mSlot.isValid(); }
    bool operator==(const ComponentHandler& other) const
	{
		return
         mComponentsManager == other.mComponentsManager &&
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
        mComponentsManager = nullptr;
    }

public:
    Slot mSlot;
    ClassId mClassId = 0;
    ComponentsManager* mComponentsManager = nullptr;
};

class ComponentsManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;

    template<class T> T_EXTENDS(T, Component)
    ComponentHandler requestComponent()
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentsArrays.contains(id))
        {
            mComponentsArrays.emplace(id, smInitialComponents);
        }

        if(mComponentsArrays.at(id).mSlotsManager.isEmpty())
        {
            mComponentsArrays.at(id).mSlotsManager.increaseSize(smInitialComponents);
            mComponentsArrays.at(id).mComponents.resize(mComponentsArrays.at(id).mSlotsManager.getSize());
        }

        ComponentHandler componentHandler(id, mComponentsArrays.at(id).mSlotsManager.requestSlot(), this);
        u32 slot = componentHandler.mSlot.getSlot();
        if(mComponentsArrays.at(id).mComponents[slot].isValid())
        {
            mComponentsArrays.at(id).mComponents[slot]->onRecycle();
        }
        else
        {
            mComponentsArrays.at(id).mComponents[slot] = OwnerPtr<Component>::moveCast(OwnerPtr<T>::newObject());
        }

        mPtrToHandler.emplace(mComponentsArrays.at(id).mComponents[slot], componentHandler);

        return componentHandler;
    }

    void removeComponent(ComponentHandler& componentHandler)
    {
        u32 slot = componentHandler.mSlot.getSlot();
        ClassId id = componentHandler.mClassId;

        if(mComponentsArrays.contains(id))
        {
            mPtrToHandler.erase(mComponentsArrays.at(id).mComponents[slot]);
            
            // mComponentsArrays.at(componentHandler.mClassId).mComponents[slot].invalidate();
            mComponentsArrays.at(id).mSlotsManager.freeSlot(componentHandler.mSlot);
        }
        componentHandler.reset();
    }

    void removeComponent(Ptr<Component> component)
    {
        // NOTE: copy!
        ComponentHandler componentHandler = mPtrToHandler.at(component);
        removeComponent(componentHandler);
    }

    Ptr<Component> getComponent(ComponentHandler componentHandler) const
    {
        u32 slot = componentHandler.mSlot.getSlot();
        return mComponentsArrays.at(componentHandler.mClassId).mComponents[slot];
    }

    Ptr<Component> getComponentFromSlot(ClassId classId, const Slot& slot) const
    {
        return mComponentsArrays.at(classId).mComponents[slot.getSlot()];
    }

private:
    class ComponentsArray
    {
    public:
        ComponentsArray(u32 reservedComponents)
        {
            mComponents.resize(reservedComponents);
            mSlotsManager.init(reservedComponents);
        }
        std::vector<OwnerPtr<Component>> mComponents;
        SlotsManager mSlotsManager;
    };

    std::unordered_map<ClassId, ComponentsArray> mComponentsArrays;
    std::unordered_map<Ptr<Component>, ComponentHandler> mPtrToHandler;

    inline static const u32 smInitialComponents = 3000;
};
REGISTER_CLASS(ComponentsManager);