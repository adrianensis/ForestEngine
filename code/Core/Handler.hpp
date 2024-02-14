#pragma once

#include "Core/StdCore.hpp"

class Handler
{
public:
    Handler() = default;
    Handler(u32 index)
    {
        mIndex = index;
        mIsValid = true;
    }

    void reset()
    {
        mIndex = 0;
        mIsValid = false;
    }

private:
    u32 mIndex = 0;
    bool mIsValid = false;

public:
    u32 getIndex() const { return mIndex; } 
    bool isValid() const { return mIsValid; } 
};