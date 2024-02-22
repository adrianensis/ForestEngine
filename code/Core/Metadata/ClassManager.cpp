#include "Core/Metadata/ClassManager.hpp"

ClassRegister::ClassRegister(const ClassDefinition& classDefinition)
{
    ClassManager::insert(classDefinition);
}

ClassRegister::ClassRegister(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback): ClassRegister(classDefinition)
{
    ClassMetadata& classMetadata = ClassManager::insert(classDefinition);
    classMetadata.mCallback = callback;
}

ClassMetadata::ClassMetadata(const ClassDefinition& classDefinition)
{
    mClassDefinition = classDefinition;
}

ClassMetadata::ClassMetadata(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback): ClassMetadata(classDefinition)
{
    mClassDefinition = classDefinition;
    mCallback = callback;
}

MemberRegister::MemberRegister(const std::string_view& ownerClassName, const MemberDefinition& memberDefinition)
{
    ClassManager::getClassMetadataInternal(ownerClassName).mMembersMap.insert_or_assign(memberDefinition.mName, MemberMetadata(memberDefinition));
}

MemberMetadata::MemberMetadata(const MemberDefinition& memberDefinition)
{
    mMemberDefinition = memberDefinition;
}

IObjectMetadata* ClassManager::instance(const std::string_view& className)
{
    return mClassMapByName.at(className).mCallback();
}

ClassMetadata& ClassManager::insert(const ClassDefinition& classDefinition)
{
    if(!mClassMapByName.contains(classDefinition.mName))
    {
        mClassMapByName.insert_or_assign(classDefinition.mName, ClassMetadata(classDefinition));
    }

    if(!mClassMapById.contains(classDefinition.mId))
    {
        mClassMapById.insert_or_assign(classDefinition.mId, &mClassMapByName.at(classDefinition.mName));
    }

    return mClassMapByName.at(classDefinition.mName);
}

ClassMetadata& ClassManager::getClassMetadataInternal(const std::string_view& className)
{
    return mClassMapByName.at(className);
}

const ClassMetadata& ClassManager::getClassMetadataByName(const std::string_view& className)
{
    return mClassMapByName.at(className);
}

const ClassMetadata& ClassManager::getClassMetadataById(const ClassId classId)
{
    return *mClassMapById.at(classId);
}
