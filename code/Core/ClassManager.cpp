#include "Core/ClassManager.hpp"

ClassRegister::ClassRegister(const std::string_view& className, ClassId classId)
{
    ClassManager::getOrCreate(className, classId);
}

ClassRegister::ClassRegister(const std::string_view& className, ClassId classId, const ClassRegisterCallback& callback): ClassRegister(className, classId)
{
    ClassManager::getOrCreate(className, classId).mCallback = callback;
}

ClassInfo::ClassInfo(const std::string_view& className, ClassId classId)
{
    mClassName = className;
}

ClassInfo::ClassInfo(const std::string_view& className, ClassId classId, const ClassRegisterCallback& callback): ClassInfo(className, classId)
{
    mClassName = className;
    mCallback = callback;
}

MemberRegister::MemberRegister(const std::string_view& ownerClassName, const std::string_view& name, const std::string_view& className, ClassId classId)
{
    MAP_INSERT(ClassManager::getClassInfoInternal(ownerClassName).mMembersMap, name, MemberInfo(name, className, classId));
}

MemberInfo::MemberInfo(const std::string_view& name, const std::string_view& className, ClassId classId)
{
    mName = name;
    mClassName = className;
    mClassId = classId;
}

ObjectBase* ClassManager::instance(const std::string_view& className)
{
    return mClassMapByName.at(className).mCallback();
}

ClassInfo& ClassManager::getOrCreate(const std::string_view& className, ClassId classId)
{
    if(!MAP_CONTAINS(mClassMapByName, className))
    {
        MAP_INSERT(mClassMapByName, className, ClassInfo(className, classId));
    }

    if(!MAP_CONTAINS(mClassMapById, classId))
    {
        MAP_INSERT(mClassMapById, classId, &mClassMapByName.at(className));
    }

    return mClassMapByName.at(className);
}

ClassInfo& ClassManager::getClassInfoInternal(const std::string_view& className)
{
    return mClassMapByName.at(className);
}

const ClassInfo& ClassManager::getClassInfo(const std::string_view& className)
{
    return getClassInfoInternal(className);
}
