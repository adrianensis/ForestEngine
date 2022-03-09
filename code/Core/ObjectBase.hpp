#pragma once

#include "Core/Metadata.hpp"
#include "Core/Macros.hpp"
#include "Core/Serialization.hpp"
#include "Core/Proxy/ProxyOwner.hpp"
#include "Core/Reference.hpp"

class ObjectBase: public ObjectMeta, public ISerializable, public ProxyOwner, public std::enable_shared_from_this<ObjectBase>
{
private:
	GENERATE_METADATA(ObjectBase)

private:
	ObjectId mObjectId = 0;

protected:
	inline static ObjectId smObjectIdCounter = 0;

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
		notifyProxy();
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
};
