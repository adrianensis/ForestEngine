#include "Core/Metadata/EnumsManager.hpp"

EnumRegister::EnumRegister(const HashedString& name, const std::vector<HashedString>& valueNames)
{
    EnumsManager::create(name, valueNames);
}

EnumDefinition::EnumDefinition(const HashedString& name, const std::vector<HashedString>& valueNames)
{
    mName = name;
    mValueNames = valueNames;
}

const EnumDefinition& EnumsManager::getEnumMetadata(const HashedString& name)
{
    return mEnumsMapByName.at(name);
}

const EnumDefinition& EnumsManager::create(const HashedString& name, const std::vector<HashedString>& valueNames)
{
    if(!mEnumsMapByName.contains(name))
    {
        mEnumsMapByName.insert_or_assign(name, EnumDefinition(name, valueNames));
    }

    return mEnumsMapByName.at(name);
}