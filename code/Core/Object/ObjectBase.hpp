#pragma once

#include "Core/StdCore.hpp"

class ObjectBase: public IMemoryTrackedObject, public ISerializable, public PointedObject, public EnablePtrFromThis
{
private:
    DECLARE_METADATA_VARIABLES(ObjectBase)
public:
    DECLARE_METADATA_METHODS(virtual, EMPTY_MACRO())

public:
	ObjectBase()
	{
		if (mObjectId == 0)
		{
			mObjectId = smObjectIdCounter++;
		}
	};

	virtual ~ObjectBase()
	{

	};

	ObjectId getObjectId() const
	{
		return mObjectId;
	}
    
	// Assignment
	DECLARE_COPY()
	{
        // empty to avoid copying mObjectId
	}

    const std::string_view& getMemoryTrackName() const override
    {
        return getClassDefinition().mName;
    };

    template <class T>
	bool isDerivedClass() const
	{
		return dynamic_cast<const T *>(this) != nullptr;
	}

	template <class T>
	bool isSameClass() const
	{
		return this->getClassDefinition().mId == T::getClassDefinitionStatic().mId;
	}

private:
	ObjectId mObjectId = 0;

protected:
    // Important: starts by 1, 0 is reserved for null
	inline static ObjectId smObjectIdCounter = 1;
};
