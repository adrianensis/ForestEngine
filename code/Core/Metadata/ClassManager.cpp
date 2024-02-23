#include "Core/Metadata/ClassManager.hpp"

ClassRegister::ClassRegister(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback)
{
    ClassManager::insert(ClassMetadata(classDefinition, callback));
}

ClassMetadata::ClassMetadata(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback)
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

void* ClassManager::instance(const std::string_view& className)
{
    return mClassMapByName.at(className).mCallback();
}

void ClassManager::insert(const ClassMetadata& classMetadata)
{
    if(!mClassMapByName.contains(classMetadata.mClassDefinition.mName))
    {
        mClassMapByName.insert_or_assign(classMetadata.mClassDefinition.mName, classMetadata);
    }

    if(!mClassMapById.contains(classMetadata.mClassDefinition.mId))
    {
        mClassMapById.insert_or_assign(classMetadata.mClassDefinition.mId, &mClassMapByName.at(classMetadata.mClassDefinition.mName));
    }
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
