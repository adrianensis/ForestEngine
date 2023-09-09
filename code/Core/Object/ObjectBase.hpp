#pragma once

#include "Core/Object/ObjectMetadata.hpp"
#include "Core/Serialization/Serialization.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Core/Memory/Pool.hpp"
#include "Core/Metadata/ClassManager.hpp"

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
