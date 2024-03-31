#pragma once

#include "Core/StdCore.hpp"
#include "Core/Assert/Assert.hpp"
#include <vector>

class ByteBuffer
{
public:
    ByteBuffer() = default;

    template<class T>
    void pushBack(const T& element)
    {
        const byte* bytePtr = reinterpret_cast<const byte*>(&element);
        mBuffer.insert(mBuffer.end(), bytePtr, bytePtr + sizeof(T));
    }
    template <class T, typename ... Args>
    void emplaceBack(Args&&... args)
    {
        T element(args...);
        pushBack(element);
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
        checkIndex(index);
        return *reinterpret_cast<T*>(&mBuffer.at(index * sizeof(T)));
    }
    template<class T>
    const T& get(u32 index) const
    {
        checkIndex(index);
        return *reinterpret_cast<const T*>(&mBuffer.at(index * sizeof(T)));
    }
    void clear()
    {
        mBuffer.clear();
    }
    u32 sizeInBytes() const
    {
        return mBuffer.size();
    }
    virtual u32 size() const
    {
        return sizeInBytes();
    }
    virtual u32 capacity() const
    {
        return mBuffer.capacity();
    }
    byte* data()
    {
        return mBuffer.data();
    }
    const byte* data() const
    {
        return mBuffer.data();
    }
    virtual void reserve(u32 size)
    {
        mBuffer.reserve(size);
    }
    virtual void resize(u32 size)
    {
        mBuffer.resize(size);
    }
    template<class T>
    void fill(const T& element)
    {
        u32 typedSize = size();
        FOR_RANGE(i, 0, typedSize)
        {
            get<T>(i) = element;
        }
    }

protected:
    void checkIndex(u32 i) const
    {
        CHECK_MSG(i >= 0, "Index < 0!");
        CHECK_MSG(i < size(), "Index out of bounds!");
    }

protected:
    std::vector<byte> mBuffer;

public:
    CRGET(Buffer)
};

class TypedByteBuffer : public ByteBuffer
{
public:
    TypedByteBuffer() = default;
    TypedByteBuffer(u32 elementSizeInBytes) : mElementSizeInBytes(elementSizeInBytes) { }

    template<class T>
    void pushBack(const T& element)
    {
        checkType<T>();
        ByteBuffer::pushBack<T>(element);
    }
    template <class T, typename ... Args>
    void emplaceBack(Args&&... args)
    {
        checkType<T>();
        ByteBuffer::emplaceBack<T, Args...>(args...);
    }
    template<class T>
    void append(const std::vector<T>& elements)
    {
        checkType<T>();
        ByteBuffer::append<T>(elements);
    }
    void append(const TypedByteBuffer& elements)
    {
        CHECK_MSG(mElementSizeInBytes == elements.mElementSizeInBytes, "Different element size!");
        ByteBuffer::append(elements);
    }
    template<class T>
    T& get(u32 index)
    {
        checkType<T>();
        return ByteBuffer::get<T>(index);
    }
    template<class T>
    const T& get(u32 index) const
    {
        checkType<T>();
        return ByteBuffer::get<T>(index);
    }
    virtual u32 size() const override
    {
        return ByteBuffer::size() / mElementSizeInBytes;
    }
    virtual u32 capacity() const override
    {
        return ByteBuffer::capacity() / mElementSizeInBytes;
    }
    virtual void reserve(u32 size) override
    {
        ByteBuffer::reserve(size * mElementSizeInBytes);
    }
    virtual void resize(u32 size) override
    {
        ByteBuffer::resize(size * mElementSizeInBytes);
    }
    template<class T>
    void fill(const T& element)
    {
        checkType<T>();
        ByteBuffer::fill<T>(element);
    }

    u32 getElementSizeInBytes() const { return mElementSizeInBytes; };

private:
    template<class T>
    void checkType() const
    {
        CHECK_MSG(mElementSizeInBytes > 0, "Type size is 0!");
        CHECK_MSG(sizeof(T) == mElementSizeInBytes, "Type size does not match!");
    }

private:
    std::vector<byte> mBuffer;
    u32 mElementSizeInBytes = 0;
};