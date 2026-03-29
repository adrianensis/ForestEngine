#pragma once

#include "Core/Core.hpp"
#include "Core/CoreMacros.hpp"

NS_BEGIN(System)

class System;
class SystemsDependencyInjection
{
public:
    void addFrom(const SystemsDependencyInjection& other)
    {
        FOR_MAP(it, other.mSystems)
        {
            mSystems.emplace(it->first, it->second);
        }
    }

    template<typename T> T_EXTENDS(T, System)
    void addSystem(T* system)
    {
        Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(classId > 0, "System has no metadata!");
        CHECK_MSG(!mSystems.contains(classId), "System already added");
        mSystems.emplace(classId, system);
    }

    template<typename T> T_EXTENDS(T, System)
    T* getSystem() const
    {
        Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
        CHECK_MSG(mSystems.contains(classId), "System not found!");
        return static_cast<T*>(mSystems.at(classId));
    }

private:
    std::unordered_map<Core::ClassId, System*> mSystems;
public:
    CRGET(Systems)
};

NS_END