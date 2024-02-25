#pragma once

#include "Core/Std.hpp"

class ObjectBase: public ISerializable, public IPoolable, public EnablePtrFromThis
{
public:
	ObjectBase()
	{
		if (mObjectId == 0)
		{
			mObjectId = smObjectIdCounter++;
		}
	}

    ObjectBase(const ObjectBase& other) { } // empty to avoid copying mObjectId
    ObjectBase(ObjectBase&& other) { } // empty to avoid copying mObjectId
	virtual ~ObjectBase() { }
    ObjectBase &operator=(const ObjectBase& other) { return *this; } // empty to avoid copying mObjectId
	ObjectId getObjectId() const { return mObjectId; }

    template <class T>
	bool isDerivedClass() const
	{
		return dynamic_cast<const T *>(this) != nullptr;
	}

	// template <class T>
	// bool isSameClass() const
	// {
	// 	return this->getClassDefinition().mId == ClassManager::getClassMetadata<T>().mClassDefinition.mId;
	// }

private:
	ObjectId mObjectId = 0;

protected:
    // Important: starts by 1, 0 is reserved for null
	inline static ObjectId smObjectIdCounter = 1;
};

REGISTER_CLASS(ObjectBase)