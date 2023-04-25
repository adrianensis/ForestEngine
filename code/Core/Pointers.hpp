#pragma once

#include "Core/Memory.hpp"

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
    static Ptr<const T> toConst()
    {
        return Ptr<const T>();
    }

    template <class OtherClass>
    static Ptr<T> cast(const Ptr<OtherClass>& other)
    {
        // TODO: static cast?
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceCounter());
    }

    template <class OtherClass>
    static Ptr<T> cast(const OwnerPtr<OtherClass>& other)
    {
        // TODO: static cast?
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceCounter());
    }

    Ptr() = default;
    Ptr(const Ptr<T>& other)
    {
        set(other.mInternalPointer, other.mReferenceCounter);
    }
    Ptr(const OwnerPtr<T>& ownerPtr)
    { 
        set(ownerPtr.mInternalPointer, ownerPtr.mReferenceCounter);
    }
    ~Ptr()
    {
        invalidate();
    }

    operator Ptr<const T>() const
    {
        return Ptr<const T>(mInternalPointer, mReferenceCounter);
    }

    operator OwnerPtr<T>() const
    {
        return OwnerPtr<T>(mInternalPointer, mReferenceCounter);
    }

    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceCounter != nullptr && (*mReferenceCounter) > 0 && mInternalPointer != nullptr; }
    void invalidate()
    {
        set(nullptr, nullptr);
    }

    DECLARE_COPY(Ptr<T>)
	{
        set(other.mInternalPointer, other.mReferenceCounter);
	}

    bool operator==(const Ptr<T>& otherRef) const
	{
		return this->mInternalPointer == otherRef.mInternalPointer;
	}

    operator bool() const
    {
        return this->isValid();
    }

private:
    Ptr(T* reference, u32* referenceCounter)
    {
        set(reference, referenceCounter);
    }
    void set(T* reference, u32* referenceCounter)
    {
        mInternalPointer = reference;
        mReferenceCounter = referenceCounter;
    }

private:
    T* mInternalPointer = nullptr;
    u32* mReferenceCounter = nullptr;

public:
    GET(InternalPointer);
    GET(ReferenceCounter);
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
    Ptr<OtherClass> getPtrToThisCasted()
	{
		return Ptr<OtherClass>::cast(mPtrToThis);
	}
    template<class OtherClass>
    Ptr<const OtherClass> getPtrToThisCasted() const
	{
		return Ptr<const OtherClass>::cast(mPtrToThis);
	}

private:
    template <class OtherClass>
    void set(const Ptr<OtherClass>& ptr)
    {
        mPtrToThis = Ptr<PointedObject>::cast(ptr);
    }
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
        return OwnerPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceCounter());
    }

    OwnerPtr() = default;
    OwnerPtr(const OwnerPtr<T>& other)
    {
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceCounter);
            increment();
        }
    }
    OwnerPtr(OwnerPtr<T>&& other)
    {
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceCounter);
            increment();
        }
    }
    explicit OwnerPtr(T* reference)
    {
        invalidate();
        set(reference, new u32());
        increment();
    }

    ~OwnerPtr()
    {
        invalidate();
    }

    operator Ptr<const T>() const
    {
        return Ptr<const T>(static_cast<const T*>(mInternalPointer), mReferenceCounter);
    }

    operator OwnerPtr<const T>() const
    {
        return OwnerPtr<const T>(static_cast<const T*>(mInternalPointer), mReferenceCounter);
    }

    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceCounter != nullptr && (*mReferenceCounter) > 0 && mInternalPointer != nullptr; }
    void invalidate()
    {
        if(mReferenceCounter)
        {
            if((*mReferenceCounter) > 0)
            {
                (*mReferenceCounter) -= 1;
            }
            if((*mReferenceCounter) == 0)
            {
                if(mInternalPointer != nullptr)
                {
                    Memory::deleteObject(mInternalPointer);
                }
                //delete mReferenceCounter;
            }
        }
        set(nullptr, mReferenceCounter);
    }

    DECLARE_COPY(OwnerPtr<T>)
	{
        invalidate();
        set(other.mInternalPointer, other.mReferenceCounter);
        increment();
	}

    operator bool() const
    {
        return this->isValid();
    }

    bool operator==(const OwnerPtr<T>& otherRef) const
	{
		return this->mInternalPointer == otherRef.mInternalPointer;
	}

    bool operator!=(const OwnerPtr<T>& otherRef) const
	{
		return (*this == otherRef);
	}

    template <typename ... Args>
	static OwnerPtr<T> newObject(Args&&... args)
	{
        return OwnerPtr<T>(Memory::newObject<T>(args...));
    }

private:
    void increment()
    {
        (*mReferenceCounter) += 1;
    }

    OwnerPtr(T* reference, u32* referenceCounter)
    {
        invalidate();
        if(reference)
        {
            set(reference, referenceCounter);
            increment();
        }
    }

    void set(T* reference, u32* referenceCounter)
    {
        mInternalPointer = reference;
        mReferenceCounter = referenceCounter;
        if(mInternalPointer)
        {
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
    u32* mReferenceCounter = nullptr;

public:
    GET(InternalPointer);
    GET(ReferenceCounter);
};

// SNIFAE

// template <template <typename > class C, typename T>
// struct get_template_type<C<T>>
// {
//     using type = T;
// };

template <typename T>
struct get_const_ptr_type
{
    using type = Ptr<const T>;
};

template<class T>
struct get_const_ptr_type<Ptr<T>>
{
    using type = Ptr<const T>;
};

template<class T>
struct get_const_ptr_type<OwnerPtr<T>>
{
    using type = Ptr<const T>;
};

// HASH
// Needed for unordered_map

namespace std {
  template<class T> struct hash<Ptr<T>> {
    size_t operator()(Ptr<T> const& pointer) const {
      return size_t(&pointer.get());
    }
  };
  
  template<class T> struct hash<OwnerPtr<T>> {
    size_t operator()(OwnerPtr<T> const& pointer) const {
      return size_t(&pointer.get());
    }
  };
}