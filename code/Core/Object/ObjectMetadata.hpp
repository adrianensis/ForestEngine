#pragma once

#include "Core/Std.hpp"
#include "Core/Metadata.hpp"
#include "Core/Hash.hpp"

// Metadata for base object
class ObjectMeta
{
private:
    DECLARE_METADATA_VARIABLES(ObjectMeta)
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
		return this->getClassId() == T::getClassIdStatic();
	}
};