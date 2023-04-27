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
class RefCountedPtrBase;
template<class T>
class SharedPtr;
template<class T>
class OwnerPtr;

template<class T>
class Ptr : public BasePtr
{
template<class S>
friend class RefCountedPtrBase;
template<class U>
friend class SharedPtr;
template<class V>
friend class OwnerPtr;
template<class W>
friend class Ptr;

public:
    template <class OtherClass>
    static Ptr<T> cast(const Ptr<OtherClass>& other)
    {
        return Ptr<T>(static_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class OtherClass>
    static Ptr<T> cast(const SharedPtr<OtherClass>& other)
    {
        return Ptr<T>(static_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class OtherClass>
    static Ptr<T> cast(const OwnerPtr<OtherClass>& other)
    {
        return Ptr<T>(static_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    Ptr(const RefCountedPtrBase<T>& refCountedPtr) {  assign(refCountedPtr); }
    // Ptr(const SharedPtr<T>& sharedPtr) {  assign(sharedPtr); }
    // Ptr(const OwnerPtr<T>& ownerPtr) {  assign(ownerPtr); }
    Ptr() = default;
    Ptr(const Ptr<T>& other) { assign(other); }
    ~Ptr() { invalidate(); }
    operator Ptr<const T>() const { return Ptr<const T>(mInternalPointer, mReferenceBlock); }
    operator SharedPtr<T>() const { return SharedPtr<T>(mInternalPointer, mReferenceBlock); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    void invalidate()
    {
        if(mReferenceBlock)
        {
            if(mReferenceBlock->isWeakReferenced())
            {
                decrement();
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

    void assign(const Ptr<T>& other)
    {
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
    
    void assign(const RefCountedPtrBase<T>& ownerPtr)
    {
        invalidate();
        if(ownerPtr.isValid())
        {
            set(ownerPtr.mInternalPointer, ownerPtr.mReferenceBlock);
        }
    }
    
    void assign(const SharedPtr<T>& sharedPtr)
    {
        invalidate();
        if(sharedPtr.isValid())
        {
            set(sharedPtr.mInternalPointer, sharedPtr.mReferenceBlock);
        }
    }

    void assign(const OwnerPtr<T>& ownerPtr)
    {
        invalidate();
        if(ownerPtr.isValid())
        {
            set(ownerPtr.mInternalPointer, ownerPtr.mReferenceBlock);
        }
    }

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
    void decrement() { mReferenceBlock->mWeakReferenceCounter -= 1; }

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
friend class RefCountedPtrBase;

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

// REF COUNTED PTR BASE
template<class T>
class RefCountedPtrBase : public BasePtr
{
template<class U>
friend class Ptr;

public:
    virtual ~RefCountedPtrBase() { invalidate(); }
    operator Ptr<const T>() const { return Ptr<const T>(static_cast<const T*>(mInternalPointer), mReferenceBlock); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    operator bool() const { return this->isValid(); }
    bool operator==(const RefCountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator!=(const RefCountedPtrBase<T>& otherRef) const { return (*this == otherRef); }
    void invalidate()
    {
        if(mReferenceBlock)
        {
            if(mReferenceBlock->isReferenced())
            {
                decrement();
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
protected:
    void init(T* reference, ReferenceBlock* referenceBlock)
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
    void increment() { mReferenceBlock->mReferenceCounter += 1; }
    void decrement() { mReferenceBlock->mReferenceCounter -= 1; }
protected:
    T* mInternalPointer = nullptr;
    ReferenceBlock* mReferenceBlock = nullptr;

public:
    GET(InternalPointer);
    GET(ReferenceBlock);
};

// SHARED PTR
template<class T>
class SharedPtr : public RefCountedPtrBase<T>
{
template<class U>
friend class Ptr;

public:
    template <class OtherClass>
    static SharedPtr<T> cast(const SharedPtr<OtherClass>& other)
    {
        return SharedPtr<T>(static_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    explicit SharedPtr(T* reference) { this->init(reference, new ReferenceBlock()); }
    SharedPtr() = default;
    SharedPtr(const SharedPtr<T>& other) { assign(other); }
    SharedPtr(SharedPtr<T>&& other) { assign(other); }
    operator SharedPtr<const T>() const { return SharedPtr<const T>(static_cast<const T*>(this->mInternalPointer), this->mReferenceBlock); }
    DECLARE_COPY(SharedPtr<T>) { assign(other); }

    template <typename ... Args>
	static SharedPtr<T> newObject(Args&&... args)
	{
        return SharedPtr<T>(Memory::newObject<T>(args...));
    }

private:

    SharedPtr(T* reference, ReferenceBlock* referenceBlock) { this->init(reference, referenceBlock); }

    void assign(const SharedPtr<T>& other)
    {
        this->invalidate();
        if(other.isValid())
        {
            this->set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
};

// OWNER PTR
template<class T>
class OwnerPtr : public RefCountedPtrBase<T>
{
public:
    template <class OtherClass>
    static OwnerPtr<T> cast(const OwnerPtr<OtherClass>& other)
    {
        return OwnerPtr<T>(static_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    explicit OwnerPtr(T* reference) { this->init(reference, new ReferenceBlock()); }
    OwnerPtr() = default;
    OwnerPtr(OwnerPtr<T>&& other) { assign(other); }
    operator OwnerPtr<const T>() const { return OwnerPtr<const T>(static_cast<const T*>(this->mInternalPointer), this->mReferenceBlock); }
    DECLARE_MOVE(OwnerPtr<T>) { assign(other); }

    template <typename ... Args>
	static OwnerPtr<T> newObject(Args&&... args)
	{
        return OwnerPtr<T>(Memory::newObject<T>(args...));
    }

private:

    OwnerPtr(T* reference, ReferenceBlock* referenceBlock) { this->init(reference, referenceBlock); }

    void assign(OwnerPtr<T>& other)
    {
        this->invalidate();
        if(other.isValid())
        {
            this->set(other.mInternalPointer, other.mReferenceBlock);
            other.invalidate();
        }
    }
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
struct get_const_ptr_type<SharedPtr<T>> { using type = Ptr<const T>; };

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
  struct hash<SharedPtr<T>> 
  {
    size_t operator()(SharedPtr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
}