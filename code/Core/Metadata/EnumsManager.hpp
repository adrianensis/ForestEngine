#pragma once

#include "Core/StdCore.hpp"
#include "Core/ConstString/ConstString.hpp"
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
inline const ConstString& EnumsManager::getEnumNameFromTemplate<EnumName>() { static ConstString name = #EnumName; return name; } \
inline static EnumRegister enumRegister_##EnumName = EnumRegister(#EnumName, { FOR_EACH_EVEN(ADD_TRAIL_COMMA, __VA_ARGS__) });

// --------------------------------------------------------
// ENUMS
// --------------------------------------------------------

class EnumRegister
{
public:
    EnumRegister(const ConstString& name, const std::vector<ConstString>& valueNames);
};

class EnumDefinition
{
public:
    EnumDefinition(const ConstString& name, const std::vector<ConstString>& valueNames);

    ConstString mName;
    std::vector<ConstString> mValueNames;
};

class EnumsManager
{
friend EnumRegister;

public:
    template <typename E>
    static const ConstString& getEnumNameFromTemplate()
    {
        return mEmptyName;
    }
    template <typename E>
    static const ConstString& toString(E enumToken)
    {
        return getEnumMetadata<E>().mValueNames[static_cast<u8>(enumToken)];
    }
    template <typename E>
    static const ConstString& toString(u8 enumToken)
    {
        return getEnumMetadata<E>().mValueNames[static_cast<u8>(enumToken)];
    }
    static const ConstString& toString(const ConstString& name, u8 enumToken)
    {
        return getEnumMetadata(name).mValueNames[enumToken];
    }
    template <typename E>
    static const EnumDefinition& getEnumMetadata()
    {
        return getEnumMetadata(getEnumNameFromTemplate<E>());
    }
    static const EnumDefinition& getEnumMetadata(const ConstString& name);
private:
    static const EnumDefinition& create(const ConstString& name, const std::vector<ConstString>& valueNames);
private:
    inline static std::unordered_map<ConstString, EnumDefinition> mEnumsMapByName;
    inline static ConstString mEmptyName;
};