#pragma once

#include "Core/Memory/Memory.hpp"

template<class T>
class RefCountedPtrBase;
template<class T>
class SharedPtr;
template<class T>
class OwnerPtr;
template<class T>
class Ptr;

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

class PointedObject
{
public:
    virtual ~PointedObject() = default;
};


// SNIFAE

// Example:
// template <template <typename > class C, typename T>
// struct get_template_type<C<T>>
// {
//     using type = T;
// };

template <typename U>
struct get_ptr_type { using type = Ptr<U>; };
template<class T>
struct get_ptr_type<Ptr<T>> { using type = Ptr<T>; };
template<class T>
struct get_ptr_type<RefCountedPtrBase<T>> { using type = Ptr<T>; };
template<class T>
struct get_ptr_type<SharedPtr<T>> { using type = Ptr<T>; };
template<class T>
struct get_ptr_type<OwnerPtr<T>> { using type = Ptr<T>; };

template <typename U>
struct get_const_ptr_type { using type = Ptr<const U>; };
template<class T>
struct get_const_ptr_type<Ptr<T>> { using type = Ptr<const T>; };
template<class T>
struct get_const_ptr_type<RefCountedPtrBase<T>> { using type = Ptr<const T>; };
template<class T>
struct get_const_ptr_type<SharedPtr<T>> { using type = Ptr<const T>; };
template<class T>
struct get_const_ptr_type<OwnerPtr<T>> { using type = Ptr<const T>; };

template <typename V>
struct get_ptr_type;

// BASE
// Needed in Core/Macros.h CGETTER_TYPE
class BasePtr
{

};

// Needed in Core/Macros.h SETTER_TYPE
class BaseOwnerPtr
{

};

// PTR

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

GENERATE_METADATA_STRUCT(Ptr<T>);

public:
    template <class OtherClass>
    static Ptr<T> cast(const Ptr<OtherClass>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class OtherClass>
    static Ptr<T> cast(const SharedPtr<OtherClass>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class OtherClass>
    static Ptr<T> cast(const OwnerPtr<OtherClass>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
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
            CHECK_MSG(mReferenceBlock->isWeakReferenced(), "Weak references are already 0!")
            decrement();
            if(!mReferenceBlock->isReferenced() && !mReferenceBlock->isWeakReferenced())
            {
                Memory::deleteObject(mReferenceBlock);
            }
        }
        set(nullptr, nullptr);
    }

    DECLARE_COPY() { assign(other); }
    bool operator==(const Ptr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const RefCountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    operator bool() const { return this->isValid(); }

private:
    Ptr(T* reference, ReferenceBlock* referenceBlock) { set(reference, referenceBlock); }

    void assign(const Ptr<T>& other)
    {
        if(*this == other) { return; }
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
    
    void assign(const RefCountedPtrBase<T>& ownerPtr)
    {
        if(*this == ownerPtr)
        {
            return;
        }
        invalidate();
        if(ownerPtr.isValid())
        {
            set(ownerPtr.mInternalPointer, ownerPtr.mReferenceBlock);
        }
    }
    
    void assign(const SharedPtr<T>& sharedPtr)
    {
        if(*this == sharedPtr)
        {
            return;
        }
        invalidate();
        if(sharedPtr.isValid())
        {
            set(sharedPtr.mInternalPointer, sharedPtr.mReferenceBlock);
        }
    }

    void assign(const OwnerPtr<T>& ownerPtr)
    {
        if(*this == ownerPtr)
        {
            return;
        }
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
        else
        {
            // if one of them or both are invalid, force set to NULL
            mInternalPointer = nullptr;
            mReferenceBlock = nullptr;
        }
    }
    
    void increment() { mReferenceBlock->mWeakReferenceCounter += 1;}
    void decrement() { mReferenceBlock->mWeakReferenceCounter -= 1;}

private:
    T* mInternalPointer = nullptr;
    ReferenceBlock* mReferenceBlock = nullptr;

public:
    GET(InternalPointer);
    GET(ReferenceBlock);
};

class EnablePtrFromThis
{
template<class U>
friend class RefCountedPtrBase;

protected:
    template<class OtherClass>
    Ptr<OtherClass> getPtrToThisCasted() { CHECK_MSG(mPtrToThis, "Invalid PtrToThis"); return Ptr<OtherClass>::cast(mPtrToThis); }
    template<class OtherClass>
    Ptr<const OtherClass> getPtrToThisCasted() const { CHECK_MSG(mPtrToThis, "Invalid PtrToThis"); return Ptr<const OtherClass>::cast(mPtrToThis); }
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
    operator Ptr<const T>() const { return Ptr<const T>(dynamic_cast<const T*>(mInternalPointer), mReferenceBlock); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { return &get(); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    operator bool() const { return this->isValid(); }
    bool operator==(const Ptr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const RefCountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    void invalidate()
    {
        if(mReferenceBlock)
        {
            CHECK_MSG(mReferenceBlock->isReferenced(), "Weak references are already 0!")
            decrement();
            
            if(!mReferenceBlock->isReferenced() and !mReferenceBlock->isWeakReferenced())
            {
                Memory::deleteObject(mReferenceBlock);
            }
            if(!mReferenceBlock->isReferenced())
            {
                Memory::deleteObject(mInternalPointer);
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
                EnablePtrFromThis* enablePtrFromThis = dynamic_cast<EnablePtrFromThis*>(reference);
                if(enablePtrFromThis)
                {
                    enablePtrFromThis->set(Ptr<T>(*this));
                }
            }
        }
        else
        {
            // if one of them or both are invalid, force set to NULL
            mInternalPointer = nullptr;
            mReferenceBlock = nullptr;
        }
    }
    void increment() { mReferenceBlock->mReferenceCounter += 1;}
    void decrement() { mReferenceBlock->mReferenceCounter -= 1;}
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

GENERATE_METADATA_STRUCT(SharedPtr<T>);
public:
    template <class OtherClass>
    static SharedPtr<T> cast(const SharedPtr<OtherClass>& other)
    {
        return SharedPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    explicit SharedPtr(T* reference) { this->init(reference, Memory::newObject<ReferenceBlock>()); }
    SharedPtr() = default;
    SharedPtr(const SharedPtr<T>& other) { assign(other); }
    SharedPtr(SharedPtr<T>&& other) { assign(other); }
    operator SharedPtr<const T>() const { return SharedPtr<const T>(dynamic_cast<const T*>(this->mInternalPointer), this->mReferenceBlock); }
    DECLARE_COPY() { assign(other); }

    template <typename ... Args>
	static SharedPtr<T> newObject(Args&&... args)
	{
        return SharedPtr<T>(Memory::newObject<T>(args...));
    }

private:

    SharedPtr(T* reference, ReferenceBlock* referenceBlock) { this->init(reference, referenceBlock); }

    void assign(const SharedPtr<T>& other)
    {
        if(*this == other) { return; }

        this->invalidate();
        if(other.isValid())
        {
            this->set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
};

// OWNER PTR
template<class T>
class OwnerPtr : public BaseOwnerPtr, public RefCountedPtrBase<T>
{
    GENERATE_METADATA_STRUCT(OwnerPtr<T>);
public:
    template <class OtherClass>
    static OwnerPtr<T> moveCast(OwnerPtr<OtherClass>& other)
    {
        OwnerPtr<T> newPtr = OwnerPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
        other.invalidate();
        return newPtr;
    }

    explicit OwnerPtr(T* reference)
    {
        this->init(reference, Memory::newObject<ReferenceBlock>());
    }
    OwnerPtr() = default;
    OwnerPtr(OwnerPtr<T>&& other) { assign(other); }
    //operator OwnerPtr<const T>() const { return OwnerPtr<const T>(dynamic_cast<const T*>(this->mInternalPointer), this->mReferenceBlock); }
    DECLARE_MOVE() { assign(other); }

    template <typename ... Args>
	static OwnerPtr<T> newObject(Args&&... args)
	{
        return OwnerPtr<T>(Memory::newObject<T>(args...));
    }

private:

    OwnerPtr(T* reference, ReferenceBlock* referenceBlock) { this->init(reference, referenceBlock); }

    void assign(OwnerPtr<T>& other)
    {
        if(*this == other)
        {
            return;
        }

        this->invalidate();
        if(other.isValid())
        {
            this->set(other.mInternalPointer, other.mReferenceBlock);
            other.invalidate();
        }
    }
};

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
  struct hash<RefCountedPtrBase<T>> 
  {
    size_t operator()(SharedPtr<T> const& pointer) const 
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

  template<class T>
  struct hash<OwnerPtr<T>> 
  {
    size_t operator()(OwnerPtr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
}