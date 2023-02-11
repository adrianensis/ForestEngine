#pragma once

#include "Core/BasicTypes.hpp"

namespace Hash
{
    // Hash Algorithm: djb2
    // http://www.cse.yorku.ca/~oz/hash.html
    constexpr static u64 hashString(const char *str, u32 size)
    {
        u64 hashResult = 5381;
        FOR_RANGE(i, 0, size)
        {
            hashResult = ((hashResult << 5) + hashResult) + str[i]; /* hash * 33 + c */
        }

        return hashResult;
    }

	static u64 hashString(std::string key)
	{
		std::hash<std::string> hash_fn;
		u64 hashResult = hash_fn(key);
		return hashResult;
	}
    
    constexpr static u64 hashString(std::string_view key)
	{
		// std::hash<std::string_view> hash_fn;
		// u64 hashResult = hash_fn(key);
		u64 hashResult = hashString(key.data(), key.size());
		return hashResult;
	}
}

// Metadata for base object
class ObjectMeta
{
private:
    GENERATE_METADATA_BASE(ObjectMeta)
public:
    DECLARE_METADATA_METHODS(virtual, NONE(0))
public:
	ObjectMeta() { };

	template <class T>
	bool isDerivedClass() const
	{
		return dynamic_cast<const T *>(this) != nullptr;
	}

	template <class T>
	bool isSameClass() const
	{
		return isDerivedClass<ObjectMeta>() and (this->getClassId() == T::getClassIdStatic());
	}

	bool hasSameClass(ObjectMeta *object) const
	{
		return this->getClassId() == object->getClassId();
	}
};
