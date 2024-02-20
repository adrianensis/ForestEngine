#pragma once

#include "Core/Std.hpp"
#include "Core/Object/Singleton.hpp"
#include "Core/Object/ObjectBase.hpp"

class SystemComponent: public ObjectBase
{
    GENERATE_METADATA(SystemComponent)

public:
    // Important: Override this in ONLY those component classes allowed to be injected into engine systems
    // MeshRenderer for RenderEngine, Script for RenderEngine, ...
    // This will automatically work in derived classes, no need to override this method in derived classes
    virtual ClassId getSystemComponentId() const { return 0; }
public:
    bool mAlreadyAddedToSystem = false;
};

class System: public ObjectBase
{
    GENERATE_METADATA(System)

public:
    void registerComponentClass(ClassId classId);
    bool isComponentClassAccepted(ClassId classId);
    virtual void addComponent(Ptr<SystemComponent> component);

private:
    std::unordered_set<ClassId> mAcceptedSystemComponentClasses;
};

#define GET_SYSTEM_PTR(...) \
    SystemsManager::getInstance().getSystem<__VA_ARGS__>()

#define GET_SYSTEM(...) \
    GET_SYSTEM_PTR(__VA_ARGS__).get()

#define CREATE_SYSTEM(...) \
    SystemsManager::getInstance().createSystem<__VA_ARGS__>();

#define REMOVE_SYSTEM(...) \
    SystemsManager::getInstance().removeSystem<__VA_ARGS__>();

class SystemsManager : public ObjectBase, public Singleton<SystemsManager>
{
    GENERATE_METADATA(SystemsManager)

public:
    template<typename T> T_EXTENDS(T, SystemComponent)
    void addComponentToSystem(Ptr<T> component)
    {
        if (component && !component->mAlreadyAddedToSystem)
        {
            ClassId componentClassId = component->getSystemComponentId();
            FOR_MAP(itSystem, mSystems)
            {
                Ptr<System> sub = (itSystem->second);
                if (sub->isComponentClassAccepted(componentClassId))
                {
                    sub->addComponent(Ptr<SystemComponent>::cast(component));
                }
            }
        }
    }

    template<typename T> T_EXTENDS(T, System)
    void createSystem()
    {
        OwnerPtr<T> newSystem = OwnerPtr<T>::newObject();
        mSystems.insert_or_assign(T::getClassDefinitionStatic().mId, OwnerPtr<System>::moveCast(newSystem));
    }

    template<typename T> T_EXTENDS(T, System)
    Ptr<T> getSystem() const
    {
        return Ptr<T>::cast(mSystems.at(T::getClassDefinitionStatic().mId));
    }

    template<typename T> T_EXTENDS(T, System)
    void removeSystem()
    {
        mSystems.erase(T::getClassDefinitionStatic().mId);
    }

private:
    std::unordered_map<ClassId, OwnerPtr<System>> mSystems;
};
