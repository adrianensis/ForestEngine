#pragma once

#include "Core/Memory/Memory.hpp"
#include "Core/Assert/Assert.hpp"
#include <atomic>

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
// Needed in Core/MetadataMacros.h CGETTER_TYPE
class BasePtr {};
// Needed in Core/MetadataMacros.h SETTER_TYPE
class BaseOwnerPtr {};

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
friend class EnablePtrFromThis;

public:
    template <class U>
    static Ptr<T> cast(const Ptr<U>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class U>
    static Ptr<T> cast(const SharedPtr<U>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class U>
    static Ptr<T> cast(const OwnerPtr<U>& other)
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
    template<class U> T_EXTENDS(T, U) 
    operator Ptr<U>() const { return Ptr<U>(dynamic_cast<U*>(this->mInternalPointer), this->mReferenceBlock); }
    operator SharedPtr<T>() const { return SharedPtr<T>(*this); }
    template<class U> T_EXTENDS(T, U) 
    operator SharedPtr<U>() const { return SharedPtr<U>(*this); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { CHECK_MSG(this->isValid(), "Invalid pointer!"); return &get(); }
    SharedPtr<T> lock() const { return SharedPtr<T>(*this); }
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

    Ptr<T>& operator=(const Ptr<T>& other)
    {
        if (this != &other)
        {
            assign(other);
        }
        return *this;
    }
    bool operator==(const Ptr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const RefCountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    operator bool() const { return this->isValid(); }

private:
    Ptr(T* reference, ReferenceBlock* referenceBlock) { set(reference, referenceBlock); }

    void assign(const Ptr<T>& other)
    {
        // if(*this == other) { return; }
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
    T* getInternalPointer() const { return mInternalPointer; };
    ReferenceBlock* getReferenceBlock() const { return mReferenceBlock; };
};

class IPointedObject
{
public:
    virtual ~IPointedObject() = default;
};

class EnablePtrFromThis: public IPointedObject
{
template<class U>
friend class RefCountedPtrBase;

protected:
    template<class OtherClass>
    Ptr<OtherClass> getPtrToThis() { return Ptr<OtherClass>::cast(mPtrToThis); }
    template<class OtherClass>
    Ptr<const OtherClass> getPtrToThis() const { return Ptr<const OtherClass>::cast(mPtrToThis); }
private:
    template <class OtherClass>
    void set(const Ptr<OtherClass>& ptr) { mPtrToThis = Ptr<IPointedObject>(dynamic_cast<IPointedObject*>(const_cast<REMOVE_CONST(OtherClass)*>(ptr.getInternalPointer())), ptr.getReferenceBlock()); CHECK_MSG(mPtrToThis, "Invalid PtrToThis");  }
    Ptr<IPointedObject> mPtrToThis;
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
    T* operator->() const { CHECK_MSG(this->isValid(), "Invalid pointer!"); return &get(); }
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
            
            if(!mReferenceBlock->isReferenced())
            {
                Memory::deleteObject(mInternalPointer);
            }
            if(!mReferenceBlock->isReferenced() && !mReferenceBlock->isWeakReferenced())
            {
                Memory::deleteObject(mReferenceBlock);
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
        mInternalPointer = nullptr;
        mReferenceBlock = nullptr;
        if(reference && referenceBlock)
        {
            mInternalPointer = reference;
            mReferenceBlock = referenceBlock;
            increment();
            if constexpr (IS_BASE_OF(EnablePtrFromThis, T))
            {
                EnablePtrFromThis* enablePtrFromThis = dynamic_cast<EnablePtrFromThis*>(const_cast<REMOVE_CONST(T)*>(reference));
                if(enablePtrFromThis)
                {
                    enablePtrFromThis->set(Ptr<T>(*this));
                }
            }
        }
    }
    void increment() { mReferenceBlock->mReferenceCounter += 1;}
    void decrement() { mReferenceBlock->mReferenceCounter -= 1;}
protected:
    T* mInternalPointer = nullptr;
    ReferenceBlock* mReferenceBlock = nullptr;

public:
    T* getInternalPointer() const { return mInternalPointer; };
    ReferenceBlock* getReferenceBlock() const { return mReferenceBlock; };
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
        return SharedPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    explicit SharedPtr(T* reference) { this->init(reference, Memory::newObject<ReferenceBlock>()); }
    SharedPtr() = default;
    SharedPtr(const Ptr<T>& other) { assign(other); }
    SharedPtr(const SharedPtr<T>& other) { assign(other); }
    SharedPtr(SharedPtr<T>&& other) { assign(other); }
    operator SharedPtr<const T>() const { return SharedPtr<const T>(dynamic_cast<const T*>(this->mInternalPointer), this->mReferenceBlock); }
    SharedPtr<T>& operator=(const SharedPtr<T>& other)
    {
        if (this != &other)
        {
            assign(other);
        }
        return *this;
    }

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
    void assign(const Ptr<T>& other)
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
class OwnerPtr : public BaseOwnerPtr, public RefCountedPtrBase<T>
{
public:
    template <class OtherClass>
    static OwnerPtr<T> moveCast(OwnerPtr<OtherClass>& other)
    {
        OwnerPtr<T> newPtr = OwnerPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
        other.invalidate();
        return newPtr;
    }

    template <class OtherClass>
    static OwnerPtr<T> moveCast(OwnerPtr<OtherClass>&& other)
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
    operator Ptr<T>() const { return Ptr<T>(dynamic_cast<T*>(this->mInternalPointer), this->mReferenceBlock); }
    template<class U> T_EXTENDS(T, U) 
    operator Ptr<U>() const { return Ptr<U>(dynamic_cast<U*>(this->mInternalPointer), this->mReferenceBlock); }
    OwnerPtr<T>& operator=(OwnerPtr<T>&& other)
    {
        if (this != &other)
        {
            assign(other);
        }
        return *this;
    }

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