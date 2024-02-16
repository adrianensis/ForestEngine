#pragma once

#include "Core/StdCore.hpp"
#include "Core/ByteBuffer.hpp"

class IPoolable
{
public:
    virtual void poolReset() { }
};

class ObjectPool;

class PoolHandler
{
public:
    PoolHandler() = default;
    PoolHandler(i32 index, ObjectPool* pool)
    {
        mIndex = index;
        mPool = pool;
    }
    template<class T> T_EXTENDS(T, IPoolable)
    T& get() const
    {
        return mPool->get(*this);
    }
    bool isValid() const { return mPool && mIndex > INVALID_INDEX; }

    bool operator==(const PoolHandler& other) const
	{
		return mPool == other.mPool && mIndex == other.mIndex;
	}

    operator bool() const
    {
        return isValid();
    }

    void reset()
    {
        mIndex = INVALID_INDEX;
        mPool = nullptr;
    }

    u32 getIndex() const { return (u32)mIndex; }

private:
    i32 mIndex = INVALID_INDEX;
    ObjectPool* mPool = nullptr;
};

class ObjectPool
{
public:
    ObjectPool(u32 elementSizeInBytes) 
    {
        mObjects = ByteBuffer(elementSizeInBytes);
        mObjects.reserve(10);
        mAvailableObjects.reserve(10);
    }

    template <class T, typename ... Args> T_EXTENDS(T, IPoolable)
    PoolHandler allocate(Args&&... args)
    {
        u32 index = 0;
        if (mAvailableObjects.empty())
        {
            mObjects.emplaceBack(args...);
            index = mObjects.size() - 1;
        }
        else
        {
            index = mAvailableObjects.back();
            mAvailableObjects.pop_back();
        }
        
        PoolHandler handle(index, this);
        return handle;
    }

    template<class T> T_EXTENDS(T, IPoolable)
    T& get(const PoolHandler& handle)
    {
        CHECK_MSG(handle.isValid(), "Invalid handle!");
        return mObjects[handle.getIndex()];
    }

    template<class T> T_EXTENDS(T, IPoolable)
    void free(PoolHandler& handle)
    {
        handle.get().poolReset();
        mAvailableObjects.push_back(handle.getIndex());
        handle.reset();
    }

private:
    ByteBuffer mObjects;
    std::vector<u32> mAvailableObjects;
};