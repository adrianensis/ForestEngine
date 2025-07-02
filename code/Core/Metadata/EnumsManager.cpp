#include "Core/Metadata/EnumsManager.hpp"

NS_BEGIN(Core)
EnumRegister::EnumRegister(InternalCPPTypeId internalCPPId, const HashedString& name, const std::vector<HashedString>& valueNames)
{
    EnumsManager::insert(internalCPPId, name, valueNames);
}

EnumDefinition::EnumDefinition(const HashedString& name, const std::vector<HashedString>& valueNames)
{
    mName = name;
    mValueNames = valueNames;
}

const EnumDefinition& EnumsManager::getEnumMetadata(InternalCPPTypeId internalCPPId)
{
    return mEnumsMapById.at(smInternalCPPTypeIdToClassId.at(internalCPPId));
}

const void EnumsManager::insert(InternalCPPTypeId internalCPPId, const HashedString& name, const std::vector<HashedString>& valueNames)
{
    if(smInternalCPPTypeIdToClassId.contains(internalCPPId))
    {
        return;
    }

    // NOTE: Different CPP enum with SAME name are not allowed.
    if(mEnumsMapById.contains(name.getHash()))
    {
        CHECK_MSG(false, "Enum already registered! {}", name.get())
    }

    smInternalCPPTypeIdToClassId.insert_or_assign(internalCPPId, name.getHash());
    mEnumsMapById.insert_or_assign(name.getHash(), EnumDefinition(name, valueNames));
}
NS_END