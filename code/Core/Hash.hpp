#pragma once

#include "Core/Std.hpp"

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