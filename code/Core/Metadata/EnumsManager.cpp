#include "Core/Metadata/EnumsManager.hpp"

EnumRegister::EnumRegister(const std::string_view& enumName, const std::vector<std::string_view>& names)
{
    EnumsManager::create(enumName, names);
}

EnumInfo::EnumInfo(const std::string_view& enumName, const std::vector<std::string_view>& names)
{
    mEnumName = enumName;
    mNames = names;
}

const EnumInfo& EnumsManager::getEnumInfo(const std::string_view& enumName)
{
    return mEnumsMapByName.at(enumName);
}

const EnumInfo& EnumsManager::create(const std::string_view& enumName, const std::vector<std::string_view>& names)
{
    if(!MAP_CONTAINS(mEnumsMapByName, enumName))
    {
        MAP_INSERT(mEnumsMapByName, enumName, EnumInfo(enumName, names));
    }

    return mEnumsMapByName.at(enumName);
}