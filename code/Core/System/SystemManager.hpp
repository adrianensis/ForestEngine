#pragma once

#include "Core/System/System.hpp"

#define GET_SYSTEM_PTR(...) \
    SystemsManager::getInstance().getSystem<__VA_ARGS__>()

#define GET_SYSTEM(...) \
    GET_SYSTEM_PTR(__VA_ARGS__).get()

#define CREATE_SYSTEM(...) \
    SystemsManager::getInstance().createSystem<__VA_ARGS__>().get();

class SystemsManager: public Singleton<SystemsManager>
{
public:
    void init();

    template<typename T> T_EXTENDS(T, System)
    WeakPtr<T> createSystem()
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(!mSystems.contains(classId), "System already created");
        OwnerPtr<T> newSystem = OwnerPtr<T>::newObject();
        mSystems.insert_or_assign(classId, OwnerPtr<System>::moveCast(newSystem));
        mSystemsInOrder.emplace_back(mSystems.at(classId));
        WeakPtr<T> systemPtr = WeakPtr<T>::cast(mSystems.at(classId));
        systemPtr->init();
        return systemPtr;
    }

    template<typename T> T_EXTENDS(T, System)
    WeakPtr<T> getSystem() const
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(mSystems.contains(classId), "System not found!");
        return WeakPtr<T>::cast(mSystems.at(classId));
    }

    void terminate();

private:
    std::unordered_map<ClassId, OwnerPtr<System>> mSystems;
    std::vector<WeakPtr<System>> mSystemsInOrder;
};
