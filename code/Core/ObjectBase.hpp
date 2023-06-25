#pragma once

#include "Core/Metadata/Metadata.hpp"
#include "Core/Macros.hpp"
#include "Core/Serialization.hpp"
#include "Core/Pointers.hpp"
#include "Core/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/Memory.hpp"

class ObjectBase: public ObjectMeta, public ISerializable, public IPoolable, public PointedObject, public EnablePtrFromThis
{
private:
	GENERATE_METADATA(ObjectBase)

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
	DECLARE_COPY(ObjectBase)
	{
        // empty to avoid copying mObjectId
	}

private:
	ObjectId mObjectId = 0;

protected:
    // Important: starts by 1, 0 is reserved for null
	inline static ObjectId smObjectIdCounter = 1;
};
