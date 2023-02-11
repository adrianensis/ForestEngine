#pragma once

#include "Core/BasicTypes.hpp"

namespace Hash
{
	static u64 hashString(std::string key)
	{
		std::hash<std::string> hash_fn;
		u64 hashString = hash_fn(key);
		return hashString;
	}
}

// Metadata for base object
class ObjectMeta
{
private:
    GENERATE_METADATA_BASE(ObjectMeta)
public:
    DECLARE_METADATA_METHODS(virtual, NONE(0), ObjectMeta)
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
