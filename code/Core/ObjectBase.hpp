#ifndef OBJECTBASE_HPP
#define OBJECTBASE_HPP

#include "Core/Metadata.hpp"
#include "Core/Macros.hpp"
#include "Core/Serialization.hpp"
#include "Core/Reference.hpp"

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

	ObjectId getObjectId()
	{
		return mObjectId;
	}

	// Assignment

	virtual void copy(const ObjectBase *other)
	{
	}

	ObjectBase& operator=(const ObjectBase& other)
	{
		copy(&other);
		return *this;
	}

private:
	ObjectId mObjectId = 0;

protected:
	inline static ObjectId smObjectIdCounter = 0;
};

#endif