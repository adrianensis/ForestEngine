#pragma once

#include <cstring>
#include <string>
#include <string_view>

#include "Core/StdMacros.hpp"

using namespace std::literals;

using f32 = float;
using f64 = double;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using byte = std::uint8_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using ClassId = u64;
using ObjectId = u64;
using HashValue = u64;

namespace Hash
{
    constexpr inline static HashValue hashString(const char *str)
    {
        HashValue hashResult = 5381;
        u32 strSize = std::strlen(str);
        FOR_RANGE(i, 0, strSize)
        {
            hashResult = ((hashResult << 5) + hashResult) + str[i]; /* hash * 33 + c */
        }

        return hashResult;
    }

    constexpr inline static HashValue hashString(const std::string& str)
	{
		return hashString(str.c_str());
	}
    
    constexpr inline static HashValue hashString(const std::string_view& str)
	{
		return hashString(str.data());
	}
}

template<>
struct std::hash<const char *> 
{
    size_t operator()(const char * str) const 
    {
        return Hash::hashString(str);
    }
};
