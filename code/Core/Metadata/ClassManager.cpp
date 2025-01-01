#include "Core/Metadata/ClassManager.hpp"

ClassRegisterHelper::ClassRegisterHelper(const ClassDefinition& classDefinition)
{
    ClassManager::insert(ClassMetadata(classDefinition));
}

ClassMetadata::ClassMetadata(const ClassDefinition& classDefinition)
{
    mClassDefinition = classDefinition;
}

MemberRegister::MemberRegister(const HashedString& ownerClassName, const MemberDefinition& memberDefinition)
{
    ClassId classId = ownerClassName.getHash();
    ClassManager::getClassMetadataByIdInternal(classId).mMembersMap.insert_or_assign(memberDefinition.mName, MemberMetadata(memberDefinition));
}

MemberMetadata::MemberMetadata(const MemberDefinition& memberDefinition)
{
    mMemberDefinition = memberDefinition;
}

void ClassManager::insert(const ClassMetadata& classMetadata)
{
    if(smClassMapById.contains(classMetadata.mClassDefinition.getId()))
    {
        return;
    }
    
    smClassMapById.insert_or_assign(classMetadata.mClassDefinition.getId(), classMetadata);
}

ClassMetadata& ClassManager::getClassMetadataByIdInternal(const ClassId classId)
{
    return smClassMapById.at(classId);
}

const ClassMetadata& ClassManager::getClassMetadataById(const ClassId classId)
{
    return getClassMetadataByIdInternal(classId);
}

void ClassManager::registerDynamicClass(u64 pointer, ClassId classId)
{
    if(smPointersToDynamicClass.contains(pointer))
    {
        CHECK_MSG(false, "Pointer already registered")
    }
    smPointersToDynamicClass.insert_or_assign(pointer, &smClassMapById.at(classId));
}

void ClassManager::unregisterDynamicClass(u64 pointer)
{
    smPointersToDynamicClass.erase(pointer);
}