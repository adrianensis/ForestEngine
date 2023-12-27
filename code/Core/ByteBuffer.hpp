#pragma once

#include "Core/Std.hpp"

class ByteBuffer
{
public:
    template<class T>
    void pushBack(const T& element)
    {
        const byte* bytePtr = reinterpret_cast<const byte*>(&element);
        mBuffer.insert(mBuffer.end(), bytePtr, bytePtr + sizeof(T));
    }
    template<class T>
    T& get(u32 index)
    {
        return internalGet<T>(index);
    }
    template<class T>
    const T& get(u32 index) const
    {
        return internalGet<T>(index);
    }
private:
    template<class T>
    T& internalGet(u32 index) const
    {
        u32 sizeOfT = sizeof(T);
        return *reinterpret_cast<T*>(&mBuffer.at(index * sizeOfT));
    }
private:
    std::vector<byte> mBuffer;
};