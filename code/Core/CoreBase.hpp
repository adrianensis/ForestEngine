#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>

#include "Core/Environment/Environment.hpp"
#include "Core/CoreMacros.hpp"

using namespace std::literals;

NS_BEGIN(Core)
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

using ObjectId = Core::u64;
using HashValue = Core::u64;

namespace Hash
{
    constexpr inline static HashValue hashString(const char *str)
    {
        HashValue hashResult = 5381;
        // NOTE: std::strlen is not constexpr
        // so we need to use std::char_traits<char>::length()
        Core::u32 strSize = std::char_traits<char>::length(str);
        // Other alternatives:
        // std::string_view(str).size()
        // constexpr std::size_t f(const char*)
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
NS_END

template<>
struct std::hash<const char *> 
{
    size_t operator()(const char * str) const 
    {
        return Core::Hash::hashString(str);
    }
};
