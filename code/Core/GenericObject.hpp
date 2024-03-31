#pragma once

#include "Core/ByteBuffer.hpp"

class GenericObjectBuffer
{
public:
    template<class T>
    void set()
    {
        mByteBuffer.clear();
        // mByteBuffer.reserve(1);
        mByteBuffer.emplaceBack<T>();
    }

    template<class T>
    T& get()
    {
        return mByteBuffer.get<T>(0);
    }

    template<class T>
    const T& get() const
    {
        return mByteBuffer.get<T>(0);
    }

private:
    ByteBuffer mByteBuffer;

public:
    const ByteBuffer& getByteBuffer() const { return mByteBuffer; }
};