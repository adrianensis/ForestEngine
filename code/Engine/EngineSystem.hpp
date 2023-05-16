#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Singleton.hpp"
#include "Core/ObjectBase.hpp"

#define REGISTER_COMPONENT_CLASS_IN_ENGINE_SYSTEM(...) \
    registerComponentClass(__VA_ARGS__::getClassIdStatic());

#define ADD_COMPONENT_TO_ENGINE_SYSTEM(engineSystemComponent) \
    EngineSystemsManager::getInstance().addComponentToEngineSystem(engineSystemComponent);

class EngineSystemComponent: public ObjectBase
{
    GENERATE_METADATA(EngineSystemComponent)

public:
    // Important: Override this in ONLY those component classes allowed to be injected into engine systems
    // Renderer for RenderEngine, Script for RenderEngine, ...
    // This will automatically work in derived classes, no need to override this method in derived classes
    virtual ClassId getEngineSystemComponentId() const { return 0; }
public:
    bool mAlreadyAddedToEngine = false;
};

class EngineSystem: public ObjectBase
{
    GENERATE_METADATA(EngineSystem)

public:
    void registerComponentClass(ClassId classId);
    bool isComponentClassAccepted(ClassId classId);
    virtual void addComponent(Ptr<EngineSystemComponent> component);

private:
    std::set<ClassId> mAcceptedEngineSystemComponentClasses;
};

#define GET_SYSTEM(...) \
    EngineSystemsManager::getInstance().getEngineSystem<__VA_ARGS__>().get()

#define CREATE_SYSTEM(...) \
    EngineSystemsManager::getInstance().createEngineSystem<__VA_ARGS__>();

#define REMOVE_SYSTEM(...) \
    EngineSystemsManager::getInstance().removeEngineSystem<__VA_ARGS__>();

class EngineSystemsManager : public ObjectBase, public Singleton<EngineSystemsManager>
{
    GENERATE_METADATA(EngineSystemsManager)

public:
    template<typename T> T_EXTENDS(T, EngineSystemComponent)
    void addComponentToEngineSystem(Ptr<T> component)
    {
        if (component and !component->mAlreadyAddedToEngine)
        {
            ClassId componentClassId = component->getEngineSystemComponentId();
            FOR_MAP(itEngineSystem, mEngineSystems)
            {
                Ptr<EngineSystem> sub = (itEngineSystem->second);
                if (sub->isComponentClassAccepted(componentClassId))
                {
                    sub->addComponent(Ptr<EngineSystemComponent>::cast(component));
                }
            }
        }
    }

    template<typename T> T_EXTENDS(T, EngineSystem)
    void createEngineSystem()
    {
        OwnerPtr<T> newEngineSystem = OwnerPtr<T>::newObject();
        MAP_INSERT(mEngineSystems, T::getClassIdStatic(), OwnerPtr<EngineSystem>::moveCast(newEngineSystem));
    }

    template<typename T> T_EXTENDS(T, EngineSystem)
    Ptr<T> getEngineSystem() const
    {
        return Ptr<T>::cast(mEngineSystems.at(T::getClassIdStatic()));
    }

    template<typename T> T_EXTENDS(T, EngineSystem)
    void removeEngineSystem()
    {
        mEngineSystems.erase(T::getClassIdStatic());
    }

private:
    std::unordered_map<ClassId, OwnerPtr<EngineSystem>> mEngineSystems;
};
