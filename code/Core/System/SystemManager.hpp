#pragma once

#include "Core/System/System.hpp"

NS_BEGIN(Core)

#define GET_SYSTEM_PTR(...) \
    Core::SystemsManager::getInstance().getSystem<__VA_ARGS__>()

#define GET_SYSTEM(...) \
    GET_SYSTEM_PTR(__VA_ARGS__).get()

#define CREATE_SYSTEM(...) \
    Core::SystemsManager::getInstance().createSystem<__VA_ARGS__>().get();

class SystemsManager: public Singleton<SystemsManager>
{
public:
    void init();

    template<typename T> T_EXTENDS(T, System)
    WeakPtr<T> createSystem()
    {
        ClassId classId = ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        LOG_TAG("SYSTEM", "Creating system: " + std::to_string(classId) + " " + ClassManager::getClassMetadata<T>().mClassDefinition.mName.get());
        CHECK_MSG(classId > 0, "System has no metadata!");
        CHECK_MSG(!mSystems.contains(classId), "System already created");
        mSystems.emplace(classId, OwnerPtr<System>::moveCast(OwnerPtr<T>::newObject()));
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
NS_END