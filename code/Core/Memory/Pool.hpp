#pragma once

#include "Core/StdCore.hpp"

class IPoolable
{
public:
    virtual void poolReset() { }
};

template<class T> T_EXTENDS(T, IPoolable)
class ObjectPool;

template<class T> T_EXTENDS(T, IPoolable)
class PoolHandle
{
public:
    PoolHandle() = default;
    PoolHandle(i32 index, ObjectPool<T>* pool);
    T& get() const;
    T* operator->() const { return &get(); }
    bool isValid() const { return mIndex >= 0; }

    bool operator==(const PoolHandle<T>& other) const
	{
		return this->mIndex == other.mIndex;
	}

    operator bool() const
    {
        return this->isValid();
    }

    void reset()
    {
        mIndex = -1;
        mPool = nullptr;
    }

private:
    i32 mIndex = -1;
    ObjectPool<T>* mPool = nullptr;

public:
    GET(Index);
};

template<class T> T_EXTENDS(T, IPoolable)
class ObjectPool
{
public:
    ObjectPool() 
    {
        mObjects.reserve(10);
        mAvailableObjects.reserve(10);
    }

    PoolHandle<T> allocate()
    {
        u32 index = 0;
        if (mAvailableObjects.empty())
        {
            mObjects.emplace_back();
            index = mObjects.size() - 1;
        }
        else
        {
            index = mAvailableObjects.back();
            mAvailableObjects.pop_back();
        }
        
        PoolHandle<T> handle(index, this);
        return handle;
    }

    T& get(i32 index)
    {
        return mObjects[index];
    }

    void free(PoolHandle<T>& handle)
    {
        handle.get().poolReset();
        mAvailableObjects.push_back(handle.getIndex());
        handle.reset();
    }

private:
    std::vector<T> mObjects;
    std::vector<u32> mAvailableObjects;
};

template<class T>
PoolHandle<T>::PoolHandle(i32 index, ObjectPool<T>* pool)
{
    mIndex = index;
    mPool = pool;
}

template<class T>
T& PoolHandle<T>::get() const
{
    CHECK_MSG(isValid(), "Invalid handle!");
    return mPool->get(mIndex);
}