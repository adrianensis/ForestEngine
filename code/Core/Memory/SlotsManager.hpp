#pragma once

#include "Core/StdCore.hpp"
#include "Core/Assert/Assert.hpp"
#include <vector>

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
        mAvailableSlots.resize(initialAvailableSlots);
        CHECK_MSG(getSize() > 0, "SlotManager is empty!");

        u32 size = getSize();
        FOR_RANGE(i, 0, size)
        {
            mAvailableSlots[i] = true;
        }
    }

    Slot requestSlot()
    {
        CHECK_MSG(getSize() > 0, "SlotManager is empty!");

        Slot slot;
        u32 size = getSize();
        FOR_RANGE(i, 0, size)
        {
            if(mAvailableSlots[i])
            {
                mAvailableSlots[i] = false;
                slot.set(i);
                break;
            }
        }

        CHECK_MSG(checkSlot(slot), "Invalid slot!");
        return slot;
    }

    void freeSlot(const Slot& slot)
    {
        CHECK_MSG(checkSlot(slot), "Invalid slot!");
        mAvailableSlots[slot.getSlot()] = true;
    }

    bool checkSlotBounds(const Slot& slot) const
    {
        bool bounds = slot.getSlot() < mAvailableSlots.size();
        return bounds;
    }

    bool checkSlot(const Slot& slot) const
    {
        bool bounds = checkSlotBounds(slot);
        bool isValid = slot.isValid();
        return bounds && isValid;
    }

    u32 getSize() const { return mAvailableSlots.size(); }
    void reset() { mAvailableSlots.clear(); }

private:
    std::vector<u32> mAvailableSlots;
};
