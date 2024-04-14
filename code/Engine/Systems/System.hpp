#pragma once

#include "Core/Std.hpp"
#include "Core/Singleton.hpp"
#include "Engine/Systems/SystemComponent.hpp"

class System: public ObjectBase
{
public:
    virtual void init() { };
    virtual void terminate() { };
    void registerComponentClass(ClassId classId);
    bool isComponentClassAccepted(ClassId classId);
    virtual void addComponent(Ptr<SystemComponent> component);
    virtual void removeComponent(Ptr<SystemComponent> component);

private:
    std::unordered_set<ClassId> mAcceptedSystemComponentClasses;
};
REGISTER_CLASS(System);

#define GET_SYSTEM_PTR(...) \
    SystemsManager::getInstance().getSystem<__VA_ARGS__>()

#define GET_SYSTEM(...) \
    GET_SYSTEM_PTR(__VA_ARGS__).get()

#define CREATE_SYSTEM(...) \
    SystemsManager::getInstance().createSystem<__VA_ARGS__>().get();

#define CHECK_SYSTEM_COMPONENT(c) \
    CHECK_MSG(isComponentClassAccepted(c->getSystemComponentId()), "Component class is not valid for this system: " + std::to_string(component->getSystemComponentId()));

class SystemsManager: public Singleton<SystemsManager>
{
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
                if (sub.isValid() && sub->isComponentClassAccepted(componentClassId))
                {
                    sub->addComponent(Ptr<SystemComponent>::cast(component));
                }
            }
        }
    }

    template<typename T> T_EXTENDS(T, SystemComponent)
    void removeComponentFromSystem(Ptr<T> component)
    {
        if (component && component->mAlreadyAddedToSystem)
        {
            ClassId componentClassId = component->getSystemComponentId();
            FOR_MAP(itSystem, mSystems)
            {
                Ptr<System> sub = (itSystem->second);
                if (sub.isValid() && sub->isComponentClassAccepted(componentClassId))
                {
                    sub->removeComponent(Ptr<SystemComponent>::cast(component));
                }
            }
        }
    }

    template<typename T> T_EXTENDS(T, System)
    Ptr<T> createSystem()
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.mId;
        CHECK_MSG(!mSystems.contains(classId), "System already created");
        OwnerPtr<T> newSystem = OwnerPtr<T>::newObject();
        mSystems.insert_or_assign(classId, OwnerPtr<System>::moveCast(newSystem));
        mSystemsInOrder.emplace_back(mSystems.at(classId));
        Ptr<T> systemPtr = Ptr<T>::cast(mSystems.at(classId));
        systemPtr->init();
        return systemPtr;
    }

    template<typename T> T_EXTENDS(T, System)
    Ptr<T> getSystem() const
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.mId;
        CHECK_MSG(mSystems.contains(classId), "System not found!");
        return Ptr<T>::cast(mSystems.at(classId));
    }

    void terminate();

private:
    std::unordered_map<ClassId, OwnerPtr<System>> mSystems;
    std::vector<Ptr<System>> mSystemsInOrder;
};
