#pragma once

#include "Core/System/System.hpp"
#include "Core/Memory/Singleton.hpp"
#include "Core/EntityComponent/Component.hpp"

NS_BEGIN(System)

#define GET_SYSTEM_PTR(...) \
    ::System::SystemsManager::getInstance().getSystem<__VA_ARGS__>()

#define GET_SYSTEM(...) \
    GET_SYSTEM_PTR(__VA_ARGS__).get()

#define CREATE_SYSTEM(...) \
    ::System::SystemsManager::getInstance().createSystem<__VA_ARGS__>().get();

class SystemsManager: public Core::Singleton<SystemsManager>
{
public:
    void init();

    template<typename T> T_EXTENDS(T, System)
    Core::WeakPtr<T> createSystem()
    {
        Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        LOG_TAG("SYSTEM", "Creating system: " + std::to_string(classId) + " " + Core::ClassManager::getClassMetadata<T>().mClassDefinition.mName.get());
        CHECK_MSG(classId > 0, "System has no metadata!");
        CHECK_MSG(!mSystems.contains(classId), "System already created");
        mSystems.emplace(classId, Core::OwnerPtr<System>::moveCast(Core::OwnerPtr<T>::newObject()));
        mSystemsInOrder.emplace_back(mSystems.at(classId));
        Core::WeakPtr<T> systemPtr = Core::WeakPtr<T>::cast(mSystems.at(classId));
        systemPtr->init();
        return systemPtr;
    }

    template<typename T> T_EXTENDS(T, System)
    Core::WeakPtr<T> getSystem() const
    {
        Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(mSystems.contains(classId), "System not found!");
        return Core::WeakPtr<T>::cast(mSystems.at(classId));
    }

    void terminate();

private:
    std::unordered_map<Core::ClassId, Core::OwnerPtr<System>> mSystems;
    std::vector<Core::WeakPtr<System>> mSystemsInOrder;
};
NS_END