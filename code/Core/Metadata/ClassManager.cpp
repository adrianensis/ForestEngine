#include "Core/Metadata/ClassManager.hpp"

ClassRegister::ClassRegister(const ClassDefinition& classDefinition)
{
    ClassManager::insert(ClassMetadata(classDefinition));
}

ClassMetadata::ClassMetadata(const ClassDefinition& classDefinition)
{
    mClassDefinition = classDefinition;
}

MemberRegister::MemberRegister(const HashedString& ownerClassName, const MemberDefinition& memberDefinition)
{
    ClassManager::getClassMetadataInternal(ownerClassName).mMembersMap.insert_or_assign(memberDefinition.mName, MemberMetadata(memberDefinition));
}

MemberMetadata::MemberMetadata(const MemberDefinition& memberDefinition)
{
    mMemberDefinition = memberDefinition;
}

void ClassManager::insert(const ClassMetadata& classMetadata)
{
    if(!smClassMapByName.contains(classMetadata.mClassDefinition.mName))
    {
        smClassMapByName.insert_or_assign(classMetadata.mClassDefinition.mName, classMetadata);
        smClassMapById.insert_or_assign(classMetadata.mClassDefinition.getId(), &smClassMapByName.at(classMetadata.mClassDefinition.mName));
    }
}

ClassMetadata& ClassManager::getClassMetadataInternal(const HashedString& className)
{
    return smClassMapByName.at(className);
}

const ClassMetadata& ClassManager::getClassMetadataByName(const HashedString& className)
{
    return smClassMapByName.at(className);
}

const ClassMetadata& ClassManager::getClassMetadataById(const ClassId classId)
{
    return *smClassMapById.at(classId);
}

void ClassManager::registerDynamicClass(u64 pointer, ClassId classId)
{
    if(!smPointersToDynamicClass.contains(pointer))
    {
        smPointersToDynamicClass.insert_or_assign(pointer, smClassMapById.at(classId));
    }
}

void ClassManager::unregisterDynamicClass(u64 pointer)
{
    smPointersToDynamicClass.erase(pointer);
}