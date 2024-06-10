#pragma once

#include "Engine/Minimal.hpp"
#include "Engine/System/System.hpp"
#include "Scene/Component.hpp"

class ComponentsManager;

template <class T>
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

    T& get() const;
    bool isValid() const { return mComponentsManager && mClassId > 0 && mSlot.isValid(); }
    T* operator->() const { return &get(); }

    bool operator==(const ComponentHandler<T>& other) const
	{
		return isValid() && other.isValid() &
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
    ComponentHandler<T> createComponent()
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mComponentBuffers.contains(id))
        {
            mComponentBuffers.emplace(id, sizeof(T), 3000);
        }

        ComponentHandler<T> componentHandler
        {
            id,
            mComponentBuffers.at(id).mSlotsManager.requestSlot()
        };

        TypedByteBuffer& typedBuffer = mComponentBuffers.at(componentHandler.mClassId).mTypedBuffer;
        u32 slot = componentHandler.mSlot.getSlot();
        typedBuffer.set<T>(slot);

        return componentHandler;
    }

    template<class T> T_EXTENDS(T, Component)
    void removeComponent(ComponentHandler<T>& componentHandler)
    {
        TypedByteBuffer& typedBuffer = mComponentBuffers.at(componentHandler.mClassId).mTypedBuffer;
        u32 slot = componentHandler.mSlot.getSlot();
        typedBuffer.remove<T>(slot);

        mComponentBuffers.at(componentHandler.mClassId).mSlotsManager.freeSlot(slot);
    }

    template<class T> T_EXTENDS(T, Component)
    T& getComponent(ComponentHandler<T> componentHandler)
    {
        TypedByteBuffer& typedBuffer = mComponentBuffers.at(componentHandler.mClassId).mTypedBuffer;
        u32 slot = componentHandler.mSlot.getSlot();
        return typedBuffer.get<T>(slot);
    }

private:
    class ComponentBuffer
    {
    public:
        ComponentBuffer(u32 elementSizeInBytes, u32 reservedComponents)
        {
            mTypedBuffer = TypedByteBuffer(elementSizeInBytes);
            mTypedBuffer.reserve(reservedComponents);
            mSlotsManager.init(reservedComponents);
        }
        TypedByteBuffer mTypedBuffer;
        SlotsManager mSlotsManager;
    };

    std::unordered_map<ClassId, ComponentBuffer> mComponentBuffers;
};
REGISTER_CLASS(ComponentsManager);

template<class T>
T& ComponentHandler<T>::get() const
{
    CHECK_MSG(isValid(), "Invalid handler!");
    return mComponentsManager->getComponent(*this);
}