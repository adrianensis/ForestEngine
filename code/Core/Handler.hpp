#pragma once

#include "Core/StdCore.hpp"

class Handler
{
public:
    Handler() = default;
    Handler(u32 index)
    {
        mIndex = index;
    }

    void reset()
    {
        mIndex = INVALID_INDEX;
    }

    u32 getIndex() const { return (u32)mIndex; } 
    bool isValid() const { return mIndex > INVALID_INDEX; } 
    operator bool() const
    {
        return isValid();
    }

private:
    i32 mIndex = INVALID_INDEX;
};