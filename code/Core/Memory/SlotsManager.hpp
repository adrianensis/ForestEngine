#pragma once

#include "Core/StdCore.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Profiler/Profiler.hpp"
#include <vector>
#include <set>

class Slot
{
public:
    void set(u32 slot)
    {
        mSlot = slot;
        mIsValid = true;
    }
    void reset()
    {
        mSlot = 0;
        mIsValid = false;
    }
private:
    u32 mSlot = 0;
    bool mIsValid = false;
public:
    u32 getSlot() const { return mSlot; }
    bool isValid() const { return mIsValid; }
};

class SlotsManager
{
public:
    void init(u32 initialAvailableSlots)
    {
        PROFILER_CPU()
        resize(initialAvailableSlots);
    }

    void resize(u32 slotsCount)
    {
        PROFILER_CPU()
        u32 currentSize = mSize;
        mSize = slotsCount;
        CHECK_MSG(mSize > 0, "SlotManager is empty!");
        FOR_RANGE(i, currentSize, mSize)
        {
            mAvailableSlots.insert(i);
        }
    }

    void increaseSize(u32 extra)
    {
        PROFILER_CPU()
        resize(mSize + extra);
    }

    Slot requestSlot()
    {
        PROFILER_CPU()
        CHECK_MSG(mSize > 0, "SlotManager is empty!");
        CHECK_MSG(!isEmpty(), "SlotManager is empty!");

        Slot slot;
        u32 i = *mAvailableSlots.begin();
        mAvailableSlots.erase(mAvailableSlots.begin());
        slot.set(i);

        CHECK_MSG(checkSlot(slot), "Invalid slot!");
        return slot;
    }

    void freeSlot(const Slot& slot)
    {
        PROFILER_CPU()
        CHECK_MSG(checkSlot(slot), "Invalid slot!");
        mAvailableSlots.insert(slot.getSlot());
    }

    bool checkSlotBounds(const Slot& slot) const
    {
        bool bounds = slot.getSlot() < mSize;
        return bounds;
    }

    bool checkSlot(const Slot& slot) const
    {
        bool bounds = checkSlotBounds(slot);
        bool isValid = slot.isValid();
        return bounds && isValid;
    }

    u32 getSize() const { return mSize; }
    u32 getMaxIndex() const { return *mAvailableSlots.rbegin(); }
    void reset() { mAvailableSlots.clear(); }
    bool isEmpty() const { return mAvailableSlots.empty(); }

private:
    std::set<u32> mAvailableSlots;
    u32 mSize = 0;
public:
};
