#include "Core/Metadata/ClassManager.hpp"
#include "Core/Assert/Assert.hpp"

NS_BEGIN(Core)

ClassDefinition::ClassDefinition(HashedString name, u32 typeSize): mName(name), mTypeSize(typeSize)
{

}

ClassDefinition::ClassDefinition(HashedString name, u32 typeSize, const std::array<HashedString, MAX_CLASS_BASES>& bases):
ClassDefinition(name, typeSize)
{
    FOR_ARRAY(i, bases)
    {
        mBases[i] = bases[i].getHash();
    }
}

ClassId ClassDefinition::getId() const
{
    return mName.getHash();
};

bool ClassDefinition::isA(ClassId classId) const
{
    ClassId thisId = getId();
    if(thisId == classId)
    {
        return true;
    }
    CHECK_MSG(mBases.empty() == false, "Error. No base classes registered for {}", this->mName.get());
    FOR_ARRAY(i, mBases)
    {
        if(mBases[i] == classId)
        {
            return true;
        }
    }

    return false;
}

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
    
    
    smClassMapById.insert_or_assign(classMetadata.mClassDefinition.getId(), classMetadata);
    smInternalCPPTypeIdToClassId.insert_or_assign(internalCPPId, &smClassMapById.at(classMetadata.mClassDefinition.getId()));
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
    smPointersToDynamicClass.insert_or_assign(pointer, &smClassMapById.at(classId));
}

void ClassManager::unregisterDynamicClass(u64 pointer)
{
    smPointersToDynamicClass.erase(pointer);
}
NS_END