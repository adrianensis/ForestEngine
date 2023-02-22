#include "Core/ClassManager.hpp"

ClassRegister::ClassRegister(const std::string_view& className)
{
    ClassManager::getOrCreate(className);
}

ClassRegister::ClassRegister(const std::string_view& className, const ClassRegisterCallback& callback): ClassRegister(className)
{
    ClassManager::getOrCreate(className).mCallback = callback;
}

ClassInfo::ClassInfo(const std::string_view& className)
{
    mClassName = className;
}

ClassInfo::ClassInfo(const std::string_view& className, const ClassRegisterCallback& callback): ClassInfo(className)
{
    mClassName = className;
    mCallback = callback;
}

MemberRegister::MemberRegister(const std::string_view& ownerClassName, const std::string_view& name, const MemberRegisterCallback& callback)
{
    MAP_INSERT(ClassManager::getOrCreate(ownerClassName).mMembersMap, name, MemberInfo(name, callback));
}

MemberInfo::MemberInfo(const std::string_view& name, const MemberRegisterCallback& callback)
{
    mName = name;
    mCallback = callback;
}

ObjectBase* ClassManager::instance(const std::string_view& className)
{
    return mClassMap.at(className).mCallback();
}

ClassInfo& ClassManager::getOrCreate(const std::string_view& className)
{
    if(!MAP_CONTAINS(mClassMap, className))
    {
        MAP_INSERT(mClassMap, className, ClassInfo(className));
    }

    return mClassMap.at(className);
}

const ClassInfo& ClassManager::getClassInfo(const std::string_view& className)
{
    return mClassMap.at(className);
}
