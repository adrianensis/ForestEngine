#pragma once

#include "Core/StdCore.hpp"
#include <vector>
#include "Core/Memory/MemoryTracking.hpp"
#include "Core/Memory/SlotsManager.hpp"

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
    virtual void clear() = 0;
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
        mElements.clear();
    }
    std::vector<T> mElements;
};

template<class BaseClass>
class Pool
{
public:
    // void init() { }
    void terminate()
    {
        FOR_MAP(it, mPools)
        {
            it->second->clear();
        }

        mPools.clear();
    }

    template<class T> T_EXTENDS(T, BaseClass)
    Slot requestElement()
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

        return slot;
    }

    void removeElement(ClassId classId, const Slot& slot)
    {
        PROFILER_CPU()

        Memory::unregisterPointer(&getElementBase(classId, slot));
        
        if(mPools.contains(classId))
        {
            mPools.at(classId)->mSlotsManager.freeSlot(slot);
        }
    }

    template<class T> T_EXTENDS(T, BaseClass)
    T& getElement(const Slot& slot) const
    {
        const ClassMetadata& classMetaData = ClassManager::getClassMetadata<T>();
        ClassId classId = classMetaData.mClassDefinition.getId();
        return *static_cast<T*>(&getElementBase(classId, slot));
    }

    BaseClass& getElementBase(ClassId classId, const Slot& slot) const
    {
        return mPools.at(classId)->at(slot.getSlot());
    }

    std::unordered_map<ClassId, OwnerPtr<PoolArrayBase<BaseClass>>> mPools;

    u32 mMaxElements = 100000;
};