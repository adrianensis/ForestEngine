#include "Core/Metadata/ClassManager.hpp"

ClassRegisterHelper::ClassRegisterHelper(InternalCPPTypeId internalCPPId, const ClassDefinition& classDefinition)
{
    ClassManager::insert(internalCPPId, ClassMetadata(classDefinition));
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

void ClassManager::insert(InternalCPPTypeId internalCPPId, const ClassMetadata& classMetadata)
{
    if(smInternalCPPTypeIdToClassId.contains(internalCPPId))
    {
        return;
    }

    // NOTE: Different CPP class with SAME name are not allowed.
    if(smClassMapById.contains(classMetadata.mClassDefinition.getId()))
    {
        CHECK_MSG(false, "Class already registered! {}", classMetadata.mClassDefinition.mName.get())
    }
    
    
    smInternalCPPTypeIdToClassId.insert_or_assign(internalCPPId, classMetadata.mClassDefinition.getId());
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