#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Hash.hpp"

// Metadata for base object
class ObjectMeta
{
private:
    GENERATE_METADATA_BASE(ObjectMeta)
public:
    DECLARE_METADATA_METHODS(virtual, EMPTY_MACRO())
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