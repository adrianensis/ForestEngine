#pragma once

#include "Core/StdCore.hpp"
#include "Core/ByteBuffer.hpp"
#include <unordered_set>
#include "Core/Memory/MemoryTracking.hpp"

template <class T>
class Pool;

template <class T>
class PoolHandler
{
public:
    PoolHandler() = default;
    PoolHandler(i32 index, Pool<T>* pool)
    {
        mIndex = index;
        mPool = pool;
        CHECK_MSG(isValid(), "Invalid handler!");
        mPool->registerHandler(*this);
    }

    PoolHandler(i32 index, const Pool<T>* pool): PoolHandler(index, const_cast<Pool<T>*>(pool))
    {
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
    Pool<T>* mPool = nullptr;
};

template <class T>
class Pool;

class IPoolable
{
template <class T>
friend class Pool;
public:
    virtual void onPoolAllocate() {};
    virtual void onPoolFree() {};
};

template <class T>
class Pool
{
public:
    Pool() = default;

    ~Pool() 
    {
        clear();
    }

    void clear() 
    {
        FOR_MAP(it, mAllocatedObjects)
        {
            internalFreeObject(*it);
        }

        mObjects.clear();
        mHandlers.clear();
        mFreeObjects.clear();
        mAllocatedObjects.clear();
    }

    template <typename ... Args>
    PoolHandler<T> allocate(Args&&... args)
    {
        u32 index = 0;
        if (mFreeObjects.empty())
        {
            mObjects.emplace_back(args...);
            mHandlers.emplace_back();
            index = mObjects.size() - 1;

            // MemoryTracking::registerNewObject<T>(&mObjects.at(index));
        }
        else
        {
            index = *mFreeObjects.begin();
            mFreeObjects.erase(index);
        }
        
        mAllocatedObjects.insert(index);
        
        PoolHandler<T> handler(index, this);

        if constexpr (IS_BASE_OF(IPoolable, T))
        {
            get(handler).onPoolAllocate();
        }

        return handler;
    }

    void registerHandler(PoolHandler<T>& handler)
    {
        CHECK_MSG(handler.isValid(), "Invalid handler!");
        mHandlers[handler.getIndex()].insert(&handler);
    }

    void unregisterHandler(PoolHandler<T>& handler)
    {
        CHECK_MSG(handler.isValid(), "Invalid handler!");
        mHandlers[handler.getIndex()].erase(&handler);
    }

    T& get(const PoolHandler<T>& handler)
    {
        CHECK_MSG(handler.isValid(), "Invalid handler!");
        return mObjects.at(handler.getIndex());
    }

    const T& get(const PoolHandler<T>& handler) const
    {
        CHECK_MSG(handler.isValid(), "Invalid handler!");
        return mObjects.at(handler.getIndex());
    }

    PoolHandler<T> getHandler(u32 index) const
    {
        PoolHandler<T> handler;
        if(mAllocatedObjects.contains(index))
        {
            handler = PoolHandler<T>(index, this);
        }
        return handler;
    }

    void free(PoolHandler<T>& handler)
    {
        if(handler.isValid())
        {
            u32 index = handler.getIndex();
            FOR_MAP(it, mHandlers[index])
            {
                (*it)->reset();
            }

            mFreeObjects.insert(index);
            mAllocatedObjects.erase(index);

            internalFreeObject(index);
        }
    }

private:
    void internalFreeObject(u32 index)
    {
        if constexpr (IS_BASE_OF(IPoolable, T))
        {
            mObjects.at(index).onPoolFree();
        }

        // MemoryTracking::unregisterDeletedObject<T>(&mObjects.at(index));
    }

private:
    std::vector<T> mObjects;
    mutable std::vector<std::unordered_set<PoolHandler<T>*>> mHandlers;
    std::unordered_set<u32> mFreeObjects;
    std::unordered_set<u32> mAllocatedObjects;
};

template<class T>
T& PoolHandler<T>::get() const
{
    CHECK_MSG(isValid(), "Invalid handler!");
    return mPool->get(*this);
}