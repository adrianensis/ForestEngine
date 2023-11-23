#include "Core/Metadata/EnumsManager.hpp"

EnumRegister::EnumRegister(const std::string_view& name, const std::vector<std::string_view>& valueNames)
{
    EnumsManager::create(name, valueNames);
}

EnumDefinition::EnumDefinition(const std::string_view& name, const std::vector<std::string_view>& valueNames)
{
    mName = name;
    mValueNames = valueNames;
}

const EnumDefinition& EnumsManager::getEnumMetadata(const std::string_view& name)
{
    return mEnumsMapByName.at(name);
}

const EnumDefinition& EnumsManager::create(const std::string_view& name, const std::vector<std::string_view>& valueNames)
{
    if(!mEnumsMapByName.contains(name))
    {
        mEnumsMapByName.insert_or_assign(name, EnumDefinition(name, valueNames));
    }

    return mEnumsMapByName.at(name);
}