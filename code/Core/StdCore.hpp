#pragma once

#include <cstdint> // std::uintptr_t
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

namespace Hash
{
    // Hash Algorithm: djb2
    // http://www.cse.yorku.ca/~oz/hash.html
    constexpr inline static u64 hashString(const char *str, u32 size)
    {
        u64 hashResult = 5381;
        FOR_RANGE(i, 0, size)
        {
            hashResult = ((hashResult << 5) + hashResult) + str[i]; /* hash * 33 + c */
        }

        return hashResult;
    }

	inline static u64 hashString(std::string key)
	{
		std::hash<std::string> hash_fn;
		u64 hashResult = hash_fn(key);
		return hashResult;
	}
    
    constexpr inline static u64 hashString(std::string_view key)
	{
		// std::hash<std::string_view> hash_fn;
		// u64 hashResult = hash_fn(key);
		u64 hashResult = hashString(key.data(), key.size());
		return hashResult;
	}
}