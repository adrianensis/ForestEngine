#pragma once

#include "Core/StdCore.hpp"
#include <unordered_map>
#include <vector>

#define DECLARE_ENUM(EnumName, ...) \
enum class EnumName : u32\
{\
    /* Notice extra MAX enum token! */ \
    FOR_EACH_ODD(ADD_TRAIL_COMMA, __VA_ARGS__) \
    MAX\
};\
template <>\
inline const std::string_view& EnumsManager::getEnumNameFromTemplate<EnumName>() { static std::string_view name = #EnumName; return name; } \
inline static EnumRegister enumRegister_##EnumName = EnumRegister(#EnumName, { FOR_EACH_EVEN(ADD_TRAIL_COMMA, __VA_ARGS__) });

// --------------------------------------------------------
// ENUMS
// --------------------------------------------------------

class EnumRegister
{
public:
    EnumRegister(const std::string_view& name, const std::vector<std::string_view>& valueNames);
};

class EnumDefinition
{
public:
    EnumDefinition(const std::string_view& name, const std::vector<std::string_view>& valueNames);

    std::string_view mName;
    std::vector<std::string_view> mValueNames;
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
        return getEnumMetadata<E>().mValueNames[static_cast<u8>(enumToken)];
    }
    template <typename E>
    static const std::string_view& toString(u8 enumToken)
    {
        return getEnumMetadata<E>().mValueNames[static_cast<u8>(enumToken)];
    }
    static const std::string_view& toString(const std::string_view& name, u8 enumToken)
    {
        return getEnumMetadata(name).mValueNames[enumToken];
    }
    template <typename E>
    static const EnumDefinition& getEnumMetadata()
    {
        return getEnumMetadata(getEnumNameFromTemplate<E>());
    }
    static const EnumDefinition& getEnumMetadata(const std::string_view& name);
private:
    static const EnumDefinition& create(const std::string_view& name, const std::vector<std::string_view>& valueNames);
private:
    inline static std::unordered_map<std::string_view, EnumDefinition> mEnumsMapByName;
    inline static std::string_view mEmptyName;
};