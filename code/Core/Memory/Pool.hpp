#pragma once

#include "Core/HashedString/HashedString.hpp"
#include "Core/StdCore.hpp"
#include <vector>
#include "Core/Memory/Memory.hpp"
#include "Core/Memory/Pointers.hpp"
#include "Core/Memory/SlotsManager.hpp"

NS_BEGIN(Core)
class PoolElementPtr
{
public:
    PoolElementPtr() = default;
    PoolElementPtr(ClassId id, Slot slot)
    {
        mClassId = id;
        mSlot = slot;
    }

    PoolElementPtr(const PoolElementPtr& other): PoolElementPtr(other.mClassId, other.mSlot)
    {
    }

    virtual ~PoolElementPtr()
    {
        reset();
    }

    PoolElementPtr& operator=(const PoolElementPtr& other)
    {
        if (this != &other)
        {
            mClassId = other.mClassId;
            mSlot = other.mSlot;
        }
        return *this;
    }

    bool isValid() const { return mClassId > 0 && mSlot.isValid(); }
    operator bool() const { return this->isValid(); }
    bool operator==(const PoolElementPtr& other) const
	{
		return
         mClassId == other.mClassId &&
         mSlot.getSlot() == other.mSlot.getSlot();
	}

    void reset()
    {
        mSlot.reset();
        mClassId = 0;
    }

public:
    Slot mSlot;
    ClassId mClassId = 0;
};

template<class BaseClass>
class PoolArrayBase
{
public:
    virtual ~PoolArrayBase() = default;
    PoolArrayBase(u32 reservedElements)
    {
        mSlotsManager.init(reservedElements);
    }
    virtual BaseClass& at(u32 index) = 0;
    virtual u32 size() const = 0;
    virtual void emplaceBack() = 0;
    virtual void clear()
    {
        mSlotsManager.reset();
    }
    SlotsManager mSlotsManager;
};

template <class T, class BaseClass> T_EXTENDS(T, BaseClass)
class PoolArray : public PoolArrayBase<BaseClass>
{
public:
    PoolArray(u32 reservedElements) : PoolArrayBase<BaseClass>(reservedElements)
    {
        PROFILER_CPU()
        mElements.reserve(reservedElements);

        #ifdef ENGINE_BUILD_DEBUG
        const ClassMetadata& baseClassMetaData = ClassManager::getClassMetadata<BaseClass>();
        mDebugStringBaseClass = baseClassMetaData.mClassDefinition.mName;
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        mDebugStringClass = classMetaData.mClassDefinition.mName;
        #endif
    }
    virtual BaseClass& at(u32 index) override
    {
        return *static_cast<BaseClass*>(&mElements.at(index));
    }
    virtual u32 size() const override
    {
        return mElements.size();
    }
    virtual void emplaceBack() override
    {
        mElements.emplace_back();
    }
    virtual void clear() override
    {
        PoolArrayBase<BaseClass>::clear();
        mElements.clear();
    }
    std::vector<T> mElements;

    #ifdef ENGINE_BUILD_DEBUG
    HashedString mDebugStringBaseClass;
    HashedString mDebugStringClass;
    #endif
};

template<class BaseClass>
class Pool
{
public:
    void init(u32 maxElements)
    {
        mMaxElements = maxElements;

        #ifdef ENGINE_BUILD_DEBUG
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<BaseClass>();
        mDebugString = classMetaData.mClassDefinition.mName;
        #endif
    }
    void terminate()
    {
        FOR_MAP(it, mPools)
        {
            FOR_ARRAY(i, it->second.get())
            {
                BaseClass& element = it->second->at(i);
                Memory::unregisterPointer(&element);
            }
            it->second->clear();
        }

        mPools.clear();
    }

    template<class T> T_EXTENDS(T, BaseClass)
    PoolElementPtr requestElement()
    {
        PROFILER_CPU()
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId id = classMetaData.mClassDefinition.getId();
        if(!mPools.contains(id))
        {
            mPools.emplace(id, OwnerPtr<PoolArrayBase<BaseClass>>::moveCast(OwnerPtr<PoolArray<T, BaseClass>>::newObject(mMaxElements)));
        }

        if(mPools.at(id)->size() == mMaxElements)
        {
            CHECK_MSG(false, "No space available for Elements!");
            // mPools.at(id).mSlotsManager.increaseSize(smInitialElements);
            // mPools.at(id).mElements.resize(mPools.at(id).mSlotsManager.getSize());
        }

        Slot slot = mPools.at(id)->mSlotsManager.requestSlot();
        if(slot.isValid())
        {
            if(slot.getSlot() == mPools.at(id)->size())
            {
                mPools.at(id)->emplaceBack();
            }

            BaseClass& element = mPools.at(id)->at(slot.getSlot());
            T* elementT = static_cast<T*>(&element);
            *elementT = T();
            Memory::registerPointer<T>(elementT);
        }
        else
        {
            CHECK_MSG(false, "Invalid Slot!");
        }

        return PoolElementPtr(id, slot);
    }

    void removeElement(const PoolElementPtr& ptr)
    {
        PROFILER_CPU()

        Memory::unregisterPointer(&getElementBase(ptr));
        
        if(mPools.contains(ptr.mClassId))
        {
            mPools.at(ptr.mClassId)->mSlotsManager.freeSlot(ptr.mSlot);
        }
    }

    template<class T> T_EXTENDS(T, BaseClass)
    T& getElement(const PoolElementPtr& ptr) const
    {
        return *static_cast<T*>(&getElementBase(ptr));
    }

    BaseClass& getElementBase(const PoolElementPtr& ptr) const
    {
        return mPools.at(ptr.mClassId)->at(ptr.mSlot.getSlot());
    }

    std::unordered_map<ClassId, OwnerPtr<PoolArrayBase<BaseClass>>> mPools;
    u32 mMaxElements = 0;

    #ifdef ENGINE_BUILD_DEBUG
    HashedString mDebugString;
    #endif
};
NS_END