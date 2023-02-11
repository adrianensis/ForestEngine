#pragma once

#include "Core/Metadata.hpp"
#include "Core/Macros.hpp"
#include "Core/Serialization.hpp"
#include "Core/Pointers.hpp"
#include "Core/ClassManager.hpp"

class ObjectBase: public ObjectMeta, public ISerializable, public std::enable_shared_from_this<ObjectBase>
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
	inline static ObjectId smObjectIdCounter = 0;
};
