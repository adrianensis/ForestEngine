#ifndef METADATA_HPP
#define METADATA_HPP

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
public:
	ObjectMeta() { };

	static std::string getClassNameStatic()
	{
		return smClassName;
	}

	static ClassId getClassIdStatic()
	{
		return smClassId;
	}

	virtual ClassId getClassId() const
	{
		return ObjectMeta::getClassIdStatic();
	}

	virtual std::string getClassName() const
	{
		return ObjectMeta::getClassNameStatic();
	}

	template <class T>
	bool isDerivedClass() const
	{
		return dynamic_cast<const T *>(this) != nullptr;
	}

	template <class T>
	bool isSameClass() const
	{
		return isDerivedClass<ObjectMeta>() && (this->getClassId() == T::getClassIdStatic());
	}

	bool hasSameClass(ObjectMeta *object) const
	{
		return this->getClassId() == object->getClassId();
	}

private:
	inline static std::string smClassName = "ObjectMeta";
	inline static ClassId smClassId = Hash::hashString(smClassName);
};

#endif