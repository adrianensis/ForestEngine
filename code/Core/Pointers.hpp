#pragma once

#include "Core/Memory.hpp"

class ReferenceBlock
{
public:
    ReferenceBlock() = default;
    bool isReferenced() const { return mReferenceCounter > 0; }
    bool isWeakReferenced() const { return mWeakReferenceCounter > 0; }
public:
    u32 mWeakReferenceCounter = 0;
    u32 mReferenceCounter = 0;
};

// BASE
// Needed in Core/Macros.h CGETTER_TYPE
class BasePtr
{

};

// PTR

template<class T>
class OwnerPtr;

template<class T>
class Ptr : public BasePtr
{
template<class U>
friend class OwnerPtr;
template<class V>
friend class Ptr;

public:
    template <class OtherClass>
    static Ptr<T> cast(const Ptr<OtherClass>& other)
    {
        // TODO: static cast?
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class OtherClass>
    static Ptr<T> cast(const OwnerPtr<OtherClass>& other)
    {
        // TODO: static cast?
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    Ptr() = default;
    Ptr(const Ptr<T>& other)
    {
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
    Ptr(const OwnerPtr<T>& ownerPtr)
    { 
        invalidate();
        if(ownerPtr.isValid())
        {
            set(ownerPtr.mInternalPointer, ownerPtr.mReferenceBlock);
        }
    }
    ~Ptr() { invalidate(); }

    operator Ptr<const T>() const { return Ptr<const T>(mInternalPointer, mReferenceBlock); }
    operator OwnerPtr<T>() const { return OwnerPtr<T>(mInternalPointer, mReferenceBlock); }

    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    void invalidate()
    {
        if(mReferenceBlock)
        {
            if(mReferenceBlock->isWeakReferenced())
            {
                (mReferenceBlock->mWeakReferenceCounter) -= 1;
            }
            if(! (mReferenceBlock->isReferenced() || mReferenceBlock->isWeakReferenced()))
            {
                delete mReferenceBlock;
            }
        }
        set(nullptr, nullptr);
    }

    DECLARE_COPY(Ptr<T>) { set(other.mInternalPointer, other.mReferenceBlock); }
    bool operator==(const Ptr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    operator bool() const { return this->isValid(); }

private:
    Ptr(T* reference, ReferenceBlock* referenceBlock) { set(reference, referenceBlock); }
    void set(T* reference, ReferenceBlock* referenceBlock)
    {
        mInternalPointer = reference;
        mReferenceBlock = referenceBlock;
        if(mInternalPointer && mReferenceBlock)
        {
            increment();
        }
    }
    void increment() { mReferenceBlock->mWeakReferenceCounter += 1; }

private:
    T* mInternalPointer = nullptr;
    ReferenceBlock* mReferenceBlock = nullptr;

public:
    GET(InternalPointer);
    GET(ReferenceBlock);
};

class PointedObject
{
public:
    virtual ~PointedObject() = default;
};

class EnablePtrFromThis
{
template<class U>
friend class OwnerPtr;

protected:
    template<class OtherClass>
    Ptr<OtherClass> getPtrToThisCasted() { return Ptr<OtherClass>::cast(mPtrToThis); }
    template<class OtherClass>
    Ptr<const OtherClass> getPtrToThisCasted() const { return Ptr<const OtherClass>::cast(mPtrToThis); }

private:
    template <class OtherClass>
    void set(const Ptr<OtherClass>& ptr) { mPtrToThis = Ptr<PointedObject>::cast(ptr); }
    Ptr<PointedObject> mPtrToThis;
};

// OWNER PTR

template<class T>
class OwnerPtr : public BasePtr
{
template<class U>
friend class Ptr;

public:

    template <class OtherClass>
    static OwnerPtr<T> cast(const OwnerPtr<OtherClass>& other)
    {
        // TODO: static cast?
        return OwnerPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    OwnerPtr() = default;
    OwnerPtr(const OwnerPtr<T>& other)
    {
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
    OwnerPtr(OwnerPtr<T>&& other)
    {
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
    explicit OwnerPtr(T* reference)
    {
        invalidate();
        if(reference)
        {
            set(reference, new ReferenceBlock());
        }
    }

    ~OwnerPtr() { invalidate(); }

    operator Ptr<const T>() const { return Ptr<const T>(static_cast<const T*>(mInternalPointer), mReferenceBlock); }
    operator OwnerPtr<const T>() const { return OwnerPtr<const T>(static_cast<const T*>(mInternalPointer), mReferenceBlock); }

    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    void invalidate()
    {
        if(mReferenceBlock)
        {
            if(mReferenceBlock->isReferenced())
            {
                (mReferenceBlock->mReferenceCounter) -= 1;
            }
            if(! mReferenceBlock->isReferenced())
            {
                if(mInternalPointer != nullptr)
                {
                    Memory::deleteObject(mInternalPointer);
                }
            }
            if(! (mReferenceBlock->isReferenced() || mReferenceBlock->isWeakReferenced()))
            {
                delete mReferenceBlock;
            }
        }
        set(nullptr, nullptr);
    }

    DECLARE_COPY(OwnerPtr<T>)
	{
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
	}

    operator bool() const { return this->isValid(); }
    bool operator==(const OwnerPtr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator!=(const OwnerPtr<T>& otherRef) const { return (*this == otherRef); }

    template <typename ... Args>
	static OwnerPtr<T> newObject(Args&&... args)
	{
        return OwnerPtr<T>(Memory::newObject<T>(args...));
    }

private:
    void increment() { mReferenceBlock->mReferenceCounter += 1; }

    OwnerPtr(T* reference, ReferenceBlock* referenceBlock)
    {
        invalidate();
        if(reference)
        {
            set(reference, referenceBlock);
        }
    }

    void set(T* reference, ReferenceBlock* referenceBlock)
    {
        mInternalPointer = reference;
        mReferenceBlock = referenceBlock;
        if(mInternalPointer && mReferenceBlock)
        {
            increment();
            if (IS_BASE_OF(EnablePtrFromThis, T))
            {
                EnablePtrFromThis* enablePtrFromThis = static_cast<EnablePtrFromThis*>(reference);
                if(enablePtrFromThis)
                {
                    enablePtrFromThis->set(Ptr<T>(*this));
                }
            }
        }
    }

private:
    T* mInternalPointer = nullptr;
    ReferenceBlock* mReferenceBlock = nullptr;

public:
    GET(InternalPointer);
    GET(ReferenceBlock);
};

// SNIFAE

// template <template <typename > class C, typename T>
// struct get_template_type<C<T>>
// {
//     using type = T;
// };

template <typename T>
struct get_const_ptr_type { using type = Ptr<const T>; };

template<class T>
struct get_const_ptr_type<Ptr<T>> { using type = Ptr<const T>; };

template<class T>
struct get_const_ptr_type<OwnerPtr<T>> { using type = Ptr<const T>; };

// HASH
// Needed for unordered_map

namespace std {
  template<class T>
  struct hash<Ptr<T>> 
  {
    size_t operator()(Ptr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
  
  template<class T>
  struct hash<OwnerPtr<T>> 
  {
    size_t operator()(OwnerPtr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
}