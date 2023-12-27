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
    void append(const std::vector<T>& elements)
    {
        if(!elements.empty())
        {
            const byte* bytePtr = reinterpret_cast<const byte*>(&elements[0]);
            mBuffer.insert(mBuffer.end(), bytePtr, bytePtr + (elements.size() * sizeof(T)));
        }
    }
    void append(const ByteBuffer& elements)
    {
        if(!elements.mBuffer.empty())
        {
            mBuffer.insert(mBuffer.end(), elements.mBuffer.begin(), elements.mBuffer.end());
        }
    }
    template<class T>
    T& get(u32 index)
    {
        u32 sizeOfT = sizeof(T);
        return *reinterpret_cast<T*>(&mBuffer.at(index * sizeOfT));
    }
    template<class T>
    const T& get(u32 index) const
    {
        u32 sizeOfT = sizeof(T);
        return *reinterpret_cast<const T*>(&mBuffer.at(index * sizeOfT));
    }
    void clear()
    {
        mBuffer.clear();
    }
    template<class T>
    u32 size() const
    {
        return mBuffer.size() / sizeof(T);
    }
    template<class T>
    u32 capacity() const
    {
        return mBuffer.capacity() / sizeof(T);
    }
    byte* data()
    {
        return mBuffer.data();
    }
    const byte* data() const
    {
        return mBuffer.data();
    }
    template<class T>
    void reserve(u32 size)
    {
        mBuffer.reserve(size * sizeof(T));
    }
    template<class T>
    void resize(u32 size)
    {
        mBuffer.resize(size * sizeof(T));
    }
    template<class T>
    void fill(const T& element)
    {
        u32 typedSize = size<T>();
        FOR_RANGE(i, 0, typedSize)
        {
            get<T>(i) = element;
        }
    }

private:
    std::vector<byte> mBuffer;
};