#pragma once

#include "Core/Std.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Core/ECS/ComponentHandler.hpp"
#include "Core/Object/ObjectBase.hpp"

class System: public ObjectBase
{
public:
    virtual void init() { };
    virtual void update() { };
    virtual void terminate() { };
    void registerComponentClass(ClassId classId);
    bool isComponentClassAccepted(ClassId classId);
    virtual void addSystemComponent(const ComponentHandler& component);
    virtual void removeSystemComponent(const ComponentHandler& component);

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
    CHECK_MSG(isComponentClassAccepted(c->getSystemComponentId()), "Component class is not valid for this system: " + std::to_string(c->getSystemComponentId()));

class SystemsManager: public Singleton<SystemsManager>
{
public:
    void addComponentToSystem(const ComponentHandler& component)
    {
        if (component && !component->mAlreadyAddedToSystem)
        {
            ClassId componentClassId = component->getSystemComponentId();
            FOR_MAP(itSystem, mSystems)
            {
                Ptr<System> sub = (itSystem->second);
                if (sub.isValid() && sub->isComponentClassAccepted(componentClassId))
                {
                    sub->addSystemComponent(component);
                }
            }
        }
    }

    void removeComponentFromSystem(const ComponentHandler& component)
    {
        if (component && component->mAlreadyAddedToSystem)
        {
            ClassId componentClassId = component->getSystemComponentId();
            FOR_MAP(itSystem, mSystems)
            {
                Ptr<System> sub = (itSystem->second);
                if (sub.isValid() && sub->isComponentClassAccepted(componentClassId))
                {
                    sub->removeSystemComponent(component);
                }
            }
        }
    }

    template<typename T> T_EXTENDS(T, System)
    Ptr<T> createSystem()
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
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
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(mSystems.contains(classId), "System not found!");
        return Ptr<T>::cast(mSystems.at(classId));
    }

    void terminate();

private:
    std::unordered_map<ClassId, OwnerPtr<System>> mSystems;
    std::vector<Ptr<System>> mSystemsInOrder;
};
