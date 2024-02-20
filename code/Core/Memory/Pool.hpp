#pragma once

#include "Core/StdCore.hpp"
#include "Core/ByteBuffer.hpp"
#include <unordered_set>

template <class T>
class ObjectPool;

template <class T>
class PoolHandler
{
public:
    PoolHandler() = default;
    PoolHandler(i32 index, ObjectPool<T>* pool)
    {
        mIndex = index;
        CHECK_MSG(pool, "Invalid pool!");
        mPool = pool;
        mPool->registerHandler(*this);
    }

    ~PoolHandler()
    {
        reset();
    }

    u32 getIndex() const { return (u32)mIndex; }
    T& get() const;
    bool isValid() const { return mPool && mIndex > INVALID_INDEX; }
    T* operator->() const { return &get(); }

    bool operator==(const PoolHandler<T>& other) const
	{
		return isValid() && other.isValid() && mPool == other.mPool && mIndex == other.mIndex;
	}

    void reset()
    {
        if(mPool)
        {
            mPool->unregisterHandler(*this);
        }
        mIndex = INVALID_INDEX;
        mPool = nullptr;
    }

private:
    i32 mIndex = INVALID_INDEX;
    ObjectPool<T>* mPool = nullptr;
};

template <class T>
class ObjectPool
{
public:
    ObjectPool() 
    {
        // mObjects.reserve(100);
        // mAvailableObjects.reserve(100);
    }

    ~ObjectPool() 
    {
        FOR_MAP(it, mHandlers)
        {
            (*it)->reset();
        }
    }

    template <typename ... Args>
    PoolHandler<T> allocate(Args&&... args)
    {
        u32 index = 0;
        if (mAvailableObjects.empty())
        {
            T object(args...);
            mObjects.emplace_back(object);
            index = mObjects.size() - 1;
        }
        else
        {
            index = mAvailableObjects.back();
            mAvailableObjects.pop_back();
        }
        
        PoolHandler<T> handle(index, this);
        return handle;
    }

    void registerHandler(PoolHandler<T>& handle)
    {
        mHandlers.insert(&handle);
    }

    void unregisterHandler(PoolHandler<T>& handle)
    {
        mHandlers.erase(&handle);
    }

    T& get(const PoolHandler<T>& handle)
    {
        CHECK_MSG(handle.isValid(), "Invalid handle!");
        return mObjects.at(handle.getIndex());
    }

    const T& get(const PoolHandler<T>& handle) const
    {
        CHECK_MSG(handle.isValid(), "Invalid handle!");
        return mObjects.at(handle.getIndex());
    }

    void free(PoolHandler<T>& handle)
    {
        mAvailableObjects.push_back(handle.getIndex());
        handle.reset();
    }

private:
    std::vector<T> mObjects;
    std::unordered_set<PoolHandler<T>*> mHandlers;
    std::vector<u32> mAvailableObjects;
};

template<class T>
T& PoolHandler<T>::get() const
{
    return mPool->get(*this);
}