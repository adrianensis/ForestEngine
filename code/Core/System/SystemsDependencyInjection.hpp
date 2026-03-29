#pragma once

#include "Core/Core.hpp"
#include "Core/CoreMacros.hpp"
#include <typeindex>
#include <typeinfo>

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
        if(typeid(T).hash_code() != mCacheTypeId)
        {
            Core::ClassId classId = Core::ClassManager::getClassMetadata<T>().mClassDefinition.getId();
            CHECK_MSG(mSystems.contains(classId), "System not found!");

            mCacheSystem = mSystems.at(classId);
            mCacheTypeId = typeid(T).hash_code();
        }

        return static_cast<T*>(mCacheSystem);
    }

private:
    std::unordered_map<Core::ClassId, System*> mSystems;
    mutable Core::InternalCPPTypeId mCacheTypeId = 0;
    mutable System* mCacheSystem = nullptr;
public:
    CRGET(Systems)
};

NS_END