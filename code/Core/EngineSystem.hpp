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

class EngineSystemsManager : public Singleton<EngineSystemsManager>
{
public:
    void addComponentToEngineSystem(Ptr<EngineSystemComponent> component);

    template<typename T>
    void createEngineSystem()
    {
        if constexpr (IS_BASE_OF(EngineSystem, T))
        {
            OwnerPtr<T> newEngineSystem = OwnerPtr<T>::newObject();
            MAP_INSERT(mEngineSystems, T::getClassIdStatic(), OwnerPtr<EngineSystem>::cast(newEngineSystem));
        }
        else
        {
            ASSERT_MSG(false, "Class is not an EngineSystem!");
        }
    }

    template<typename T>
    Ptr<T> getEngineSystem() const
    {
        if constexpr (IS_BASE_OF(EngineSystem, T))
        {
            return Ptr<T>::cast(mEngineSystems.at(T::getClassIdStatic()));
        }
        else
        {
            ASSERT_MSG(false, "Class is not an EngineSystem!");
        }

        return Ptr<T>();
    }

    template<typename T>
    void removeEngineSystem()
    {
        if constexpr (IS_BASE_OF(EngineSystem, T))
        {
            mEngineSystems.erase(T::getClassIdStatic());
        }
        else
        {
            ASSERT_MSG(false, "Class is not an EngineSystem!");
        }
    }

private:
    std::unordered_map<ClassId, OwnerPtr<EngineSystem>> mEngineSystems;
};
