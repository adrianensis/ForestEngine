#include "Core/Metadata/ClassManager.hpp"

ClassRegister::ClassRegister(const std::string_view& className, ClassId classId)
{
    ClassManager::insert(className, classId);
}

ClassRegister::ClassRegister(const std::string_view& className, ClassId classId, const ClassRegisterCallback& callback): ClassRegister(className, classId)
{
    ClassManager::insert(className, classId);
    ClassManager::getClassInfoInternal(className).mCallback = callback;
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
    ClassManager::getClassInfoInternal(ownerClassName).mMembersMap.insert_or_assign(name, MemberInfo(name, className, classId));
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

ClassInfo& ClassManager::insert(const std::string_view& className, ClassId classId)
{
    if(!mClassMapByName.contains(className))
    {
        mClassMapByName.insert_or_assign(className, ClassInfo(className, classId));
    }

    if(!mClassMapById.contains(classId))
    {
        mClassMapById.insert_or_assign(classId, &mClassMapByName.at(className));
    }

    return mClassMapByName.at(className);
}

ClassInfo& ClassManager::getClassInfoInternal(const std::string_view& className)
{
    return mClassMapByName.at(className);
}

const ClassInfo& ClassManager::getClassInfoByClassName(const std::string_view& className)
{
    return mClassMapByName.at(className);
}

const ClassInfo& ClassManager::getClassInfoByClassId(const ClassId classId)
{
    return *mClassMapById.at(classId);
}
