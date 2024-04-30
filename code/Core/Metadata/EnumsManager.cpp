#include "Core/Metadata/EnumsManager.hpp"

EnumRegister::EnumRegister(const ConstString& name, const std::vector<ConstString>& valueNames)
{
    EnumsManager::create(name, valueNames);
}

EnumDefinition::EnumDefinition(const ConstString& name, const std::vector<ConstString>& valueNames)
{
    mName = name;
    mValueNames = valueNames;
}

const EnumDefinition& EnumsManager::getEnumMetadata(const ConstString& name)
{
    return mEnumsMapByName.at(name);
}

const EnumDefinition& EnumsManager::create(const ConstString& name, const std::vector<ConstString>& valueNames)
{
    if(!mEnumsMapByName.contains(name))
    {
        mEnumsMapByName.insert_or_assign(name, EnumDefinition(name, valueNames));
    }

    return mEnumsMapByName.at(name);
}