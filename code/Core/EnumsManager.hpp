#pragma once

#include "Core/BasicTypes.hpp"

#define DECLARE_ENUM(EnumName, ...) \
enum class EnumName : u8\
{\
    /* Notice extra MAX enum token! */ \
    FOR_EACH(ADD_TRAIL_COMMA, __VA_ARGS__ , MAX) \
};\
template <>\
inline const std::string_view& EnumsManager::getEnumNameFromTemplate<EnumName>() { static std::string_view enumName = #EnumName; return enumName; } \
inline static EnumRegister enumRegister_##EnumName = EnumRegister(#EnumName, { FOR_EACH(TO_STRING_AND_ADD_TRAIL_COMMA, __VA_ARGS__) });

// --------------------------------------------------------
// ENUMS
// --------------------------------------------------------

class EnumRegister
{
public:
    EnumRegister(const std::string_view& enumName, const std::vector<std::string_view>& names);
};

class EnumInfo
{
public:
    EnumInfo(const std::string_view& enumName, const std::vector<std::string_view>& names);

    std::string_view mEnumName;
    std::vector<std::string_view> mNames;
};

class EnumsManager
{
friend EnumRegister;

public:
    template <typename E>
    static const std::string_view& getEnumNameFromTemplate()
    {
        return mEmptyName;
    }
    template <typename E>
    static const std::string_view& toString(E enumToken)
    {
        return getEnumInfo<E>().mNames[static_cast<u8>(enumToken)];
    }
    static const std::string_view& toString(const std::string_view& enumName, u8 enumToken)
    {
        return getEnumInfo(enumName).mNames[enumToken];
    }
    template <typename E>
    static const EnumInfo& getEnumInfo()
    {
        return getEnumInfo(getEnumNameFromTemplate<E>());
    }
    static const EnumInfo& getEnumInfo(const std::string_view& enumName);
private:
    static const EnumInfo& create(const std::string_view& enumName, const std::vector<std::string_view>& names);
private:
    inline static std::unordered_map<std::string_view, EnumInfo> mEnumsMapByName;
    inline static std::string_view mEmptyName;
};