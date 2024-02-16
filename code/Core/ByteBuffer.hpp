#pragma once

#include "Core/StdCore.hpp"
#include "Core/Assert/Assert.hpp"
#include <vector>

class ByteBuffer
{
public:
    ByteBuffer(u32 elementSizeInBytes) : mElementSizeInBytes(elementSizeInBytes) { }

    template<class T>
    void pushBack(const T& element)
    {
        checkType<T>();
        const byte* bytePtr = reinterpret_cast<const byte*>(&element);
        mBuffer.insert(mBuffer.end(), bytePtr, bytePtr + mElementSizeInBytes);
    }
    template<class T>
    void append(const std::vector<T>& elements)
    {
        checkType<T>();
        if(!elements.empty())
        {
            const byte* bytePtr = reinterpret_cast<const byte*>(&elements[0]);
            mBuffer.insert(mBuffer.end(), bytePtr, bytePtr + (elements.size() * mElementSizeInBytes));
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
        checkType<T>();
        return *reinterpret_cast<T*>(&mBuffer.at(index * mElementSizeInBytes));
    }
    template<class T>
    const T& get(u32 index) const
    {
        checkType<T>();
        return *reinterpret_cast<const T*>(&mBuffer.at(index * mElementSizeInBytes));
    }
    void clear()
    {
        mBuffer.clear();
    }
    u32 size() const
    {
        return mBuffer.size() / mElementSizeInBytes;
    }
    u32 capacity() const
    {
        return mBuffer.capacity() / mElementSizeInBytes;
    }
    byte* data()
    {
        return mBuffer.data();
    }
    const byte* data() const
    {
        return mBuffer.data();
    }
    void reserve(u32 size)
    {
        mBuffer.reserve(size * mElementSizeInBytes);
    }
    void resize(u32 size)
    {
        mBuffer.resize(size * mElementSizeInBytes);
    }
    template<class T>
    void fill(const T& element)
    {
        checkType<T>();
        u32 typedSize = size();
        FOR_RANGE(i, 0, typedSize)
        {
            get<T>(i) = element;
        }
    }

    u32 getElementSizeInBytes() const { return mElementSizeInBytes; };

private:
    template<class T>
    void checkType() const
    {
        CHECK_MSG(sizeof(T) == mElementSizeInBytes, "Type size does not match!");
    }

private:
    std::vector<byte> mBuffer;
    u32 mElementSizeInBytes = 0;
};