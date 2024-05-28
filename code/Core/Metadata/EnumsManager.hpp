#pragma once

#include "Core/StdCore.hpp"
#include "Core/HashedString/HashedString.hpp"
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
inline const HashedString& EnumsManager::getEnumNameFromTemplate<EnumName>() { static HashedString name = #EnumName; return name; } \
inline static EnumRegister enumRegister_##EnumName = EnumRegister(#EnumName, { FOR_EACH_EVEN(ADD_TRAIL_COMMA, __VA_ARGS__) });

// --------------------------------------------------------
// ENUMS
// --------------------------------------------------------

class EnumRegister
{
public:
    EnumRegister(const HashedString& name, const std::vector<HashedString>& valueNames);
};

class EnumDefinition
{
public:
    EnumDefinition(const HashedString& name, const std::vector<HashedString>& valueNames);

    HashedString mName;
    std::vector<HashedString> mValueNames;
};

class EnumsManager
{
friend EnumRegister;

public:
    template <typename E>
    static const HashedString& getEnumNameFromTemplate()
    {
        return mEmptyName;
    }
    template <typename E>
    static const HashedString& toString(E enumToken)
    {
        return getEnumMetadata<E>().mValueNames[static_cast<u8>(enumToken)];
    }
    template <typename E>
    static const HashedString& toString(u8 enumToken)
    {
        return getEnumMetadata<E>().mValueNames[static_cast<u8>(enumToken)];
    }
    static const HashedString& toString(const HashedString& name, u8 enumToken)
    {
        return getEnumMetadata(name).mValueNames[enumToken];
    }
    template <typename E>
    static const EnumDefinition& getEnumMetadata()
    {
        return getEnumMetadata(getEnumNameFromTemplate<E>());
    }
    static const EnumDefinition& getEnumMetadata(const HashedString& name);
private:
    static const EnumDefinition& create(const HashedString& name, const std::vector<HashedString>& valueNames);
private:
    inline static std::unordered_map<HashedString, EnumDefinition> mEnumsMapByName;
    inline static HashedString mEmptyName;
};