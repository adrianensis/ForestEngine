#ifndef OBJECTBASE_HPP
#define OBJECTBASE_HPP

#include "Core/Metadata.hpp"
#include "Core/Macros.hpp"
#include "Core/Serialization.hpp"
#include "Core/Pointers.hpp"

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

	virtual u64 getHash() const
	{
		return std::hash<ObjectId>()(mObjectId * 33) + Hash::hashString(getClassNameStatic());
	}

	// Assignment
	DECLARE_COPY(ObjectBase)
	{

	}

private:
	ObjectId mObjectId = 0;

protected:
	inline static ObjectId smObjectIdCounter = 0;
};

#endif