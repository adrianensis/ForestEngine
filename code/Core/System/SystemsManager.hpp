#pragma once

#include "Core/System/System.hpp"
#include "Core/Memory/Singleton.hpp"

NS_BEGIN(System)

#define GET_SYSTEM_PTR(...) \
    ::System::SystemsManager::getInstance().getSystem<__VA_ARGS__>()

#define GET_SYSTEM(...) \
    (*GET_SYSTEM_PTR(__VA_ARGS__))

#define CREATE_SYSTEM(...) \
    ::System::SystemsManager::getInstance().createSystem<__VA_ARGS__>()

class SystemsManager: public Core::Singleton<SystemsManager>
{
public:
    void init();

    template<typename T> T_EXTENDS(T, System)
    T* createSystem()
    {
        Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        LOG_TAG("SYSTEM", "Creating system: " + std::to_string(classId) + " " + Core::ClassManager::getClassMetadata<T>().mClassDefinition.mName.get());
        CHECK_MSG(classId > 0, "System has no metadata!");
        CHECK_MSG(!mSystems.contains(classId), "System already created");
        T* newSystem = Core::Memory::newObject<T>();
        mSystems.emplace(classId, newSystem);
        mSystemsInOrder.emplace_back(newSystem);
        return newSystem;
    }

    template<typename T> T_EXTENDS(T, System)
    T* getSystem() const
    {
        Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(mSystems.contains(classId), "System not found!");
        return static_cast<T*>(mSystems.at(classId));
    }

    void terminate();

private:
    std::unordered_map<Core::ClassId, System*> mSystems;
    std::vector<System*> mSystemsInOrder;
};
NS_END