#pragma once

#include "Core/Memory/Memory.hpp"
#include "Core/Assert/Assert.hpp"
#include <atomic>

NS_BEGIN(Core)

template<class T>
class CountedPtrBase;
template<class T>
class SharedPtr;
template<class T>
class OwnerPtr;
template<class T>
class WeakPtr;
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
struct get_ptr_type { using type = WeakPtr<U>; };
template<class T>
struct get_ptr_type<Ptr<T>> { using type = Ptr<T>; };
template<class T>
struct get_ptr_type<WeakPtr<T>> { using type = WeakPtr<T>; };
template<class T>
struct get_ptr_type<CountedPtrBase<T>> { using type = WeakPtr<T>; };
template<class T>
struct get_ptr_type<SharedPtr<T>> { using type = WeakPtr<T>; };
template<class T>
struct get_ptr_type<OwnerPtr<T>> { using type = WeakPtr<T>; };

template <typename U>
struct get_const_ptr_type { using type = WeakPtr<const U>; };
template<class T>
struct get_const_ptr_type<Ptr<T>> { using type = Ptr<const T>; };
template<class T>
struct get_const_ptr_type<WeakPtr<T>> { using type = WeakPtr<const T>; };
template<class T>
struct get_const_ptr_type<CountedPtrBase<T>> { using type = WeakPtr<const T>; };
template<class T>
struct get_const_ptr_type<SharedPtr<T>> { using type = WeakPtr<const T>; };
template<class T>
struct get_const_ptr_type<OwnerPtr<T>> { using type = WeakPtr<const T>; };

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
friend class CountedPtrBase;
template<class U>
friend class SharedPtr;
template<class V>
friend class OwnerPtr;
template<class W>
friend class WeakPtr;
friend class EnableWeakPtrToThis;

public:
    Ptr(T* reference) { set(reference); }

    template <class U>
    static Ptr<T> cast(const Ptr<U>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()));
    }

    template <class U>
    static Ptr<T> cast(const WeakPtr<U>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()));
    }

    template <class U>
    static Ptr<T> cast(const SharedPtr<U>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()));
    }

    template <class U>
    static Ptr<T> cast(const OwnerPtr<U>& other)
    {
        return Ptr<T>(dynamic_cast<T*>(other.getInternalPointer()));
    }

    Ptr(const CountedPtrBase<T>& refCountedPtr) {  assign(refCountedPtr); }
    // WeakPtr(const SharedPtr<T>& sharedPtr) {  assign(sharedPtr); }
    // WeakPtr(const OwnerPtr<T>& ownerPtr) {  assign(ownerPtr); }
    Ptr() = default;
    Ptr(const Ptr<T>& other) { assign(other); }
    ~Ptr() { invalidate(); }
    operator Ptr<const T>() const { return Ptr<const T>(mInternalPointer); }
    template<class U> T_EXTENDS(T, U) 
    operator Ptr<U>() const { return Ptr<U>(dynamic_cast<U*>(this->mInternalPointer)); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { CHECK_MSG(this->isValid(), "Invalid pointer!"); return &get(); }
    bool isValid() const { return mInternalPointer != nullptr; }
    void invalidate()
    {
        set(nullptr);
    }

    Ptr<T>& operator=(const Ptr<T>& other)
    {
        if (this != &other)
        {
            assign(other);
        }
        return *this;
    }
    Ptr<T>& operator=(const WeakPtr<T>& other)
    {
        // if (this != &other)
        {
            assign(other);
        }
        return *this;
    }
    Ptr<T>& operator=(const CountedPtrBase<T>& other)
    {
        // if (this != &other)
        {
            assign(other);
        }
        return *this;
    }
    bool operator==(const Ptr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const WeakPtr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const CountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    operator bool() const { return this->isValid(); }

private:
    void assign(const Ptr<T>& other)
    {
        // if(*this == other) { return; }
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer);
        }
    }
    
    void assign(const WeakPtr<T>& other)
    {
        // if(*this == other) { return; }
        invalidate();
        if(other.isValid())
        {
            set(other.getInternalPointer());
        }
    }
    
    void assign(const CountedPtrBase<T>& ownerPtr)
    {
        if(*this == ownerPtr)
        {
            return;
        }
        invalidate();
        if(ownerPtr.isValid())
        {
            set(ownerPtr.getInternalPointer());
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
            set(sharedPtr.getInternalPointer());
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
            set(ownerPtr.getInternalPointer());
        }
    }

    void set(T* reference)
    {
        mInternalPointer = reference;
    }

private:
    T* mInternalPointer = nullptr;

public:
    T* getInternalPointer() const { return mInternalPointer; };
};

// WEAK PTR

template<class T>
class WeakPtr : public BasePtr
{
template<class S>
friend class CountedPtrBase;
template<class U>
friend class SharedPtr;
template<class V>
friend class OwnerPtr;
template<class W>
friend class WeakPtr;
friend class EnableWeakPtrToThis;

public:
    template <class U>
    static WeakPtr<T> cast(const WeakPtr<U>& other)
    {
        return WeakPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class U>
    static WeakPtr<T> cast(const SharedPtr<U>& other)
    {
        return WeakPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    template <class U>
    static WeakPtr<T> cast(const OwnerPtr<U>& other)
    {
        return WeakPtr<T>(dynamic_cast<T*>(other.getInternalPointer()), other.getReferenceBlock());
    }

    WeakPtr(const CountedPtrBase<T>& refCountedPtr) {  assign(refCountedPtr); }
    // WeakPtr(const SharedPtr<T>& sharedPtr) {  assign(sharedPtr); }
    // WeakPtr(const OwnerPtr<T>& ownerPtr) {  assign(ownerPtr); }
    WeakPtr() = default;
    WeakPtr(const WeakPtr<T>& other) { assign(other); }
    ~WeakPtr() { invalidate(); }
    operator WeakPtr<const T>() const { return WeakPtr<const T>(mInternalPointer, mReferenceBlock); }
    template<class U> T_EXTENDS(T, U) 
    operator WeakPtr<U>() const { return WeakPtr<U>(dynamic_cast<U*>(this->mInternalPointer), this->mReferenceBlock); }
    template<class U> T_EXTENDS(T, U) 
    operator Ptr<U>() const { return Ptr<U>(dynamic_cast<U*>(this->mInternalPointer)); }
    operator Ptr<T>() const { return Ptr<T>(mInternalPointer); }
    operator SharedPtr<T>() const { return SharedPtr<T>(*this); }
    template<class U> T_EXTENDS(T, U) 
    operator SharedPtr<U>() const { return SharedPtr<U>(*this); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { CHECK_MSG(this->isValid(), "Invalid pointer!"); return &get(); }
    SharedPtr<T> lock() const { return SharedPtr<T>(*this); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    void invalidate()
    {
        internalInvalidate(true);
    }

    WeakPtr<T>& operator=(const WeakPtr<T>& other)
    {
        if (this != &other)
        {
            assign(other);
        }
        return *this;
    }
    bool operator==(const WeakPtr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const CountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    operator bool() const { return this->isValid(); }

private:
    WeakPtr(T* reference, ReferenceBlock* referenceBlock) { set(reference, referenceBlock); }

    void assign(const WeakPtr<T>& other)
    {
        // if(*this == other) { return; }
        invalidate();
        if(other.isValid())
        {
            set(other.mInternalPointer, other.mReferenceBlock);
        }
    }
    
    void assign(const CountedPtrBase<T>& ownerPtr)
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

    void internalInvalidate(bool shouldDeleteReferenceBlock)
    {
        if(mReferenceBlock)
        {
            CHECK_MSG(mReferenceBlock->isWeakReferenced(), "Weak references are already 0!")
            decrement();
            
            // INFO: When a OwnerPtr is deleted, if the class is EnableWeakPtrToThis, it could first delete mReferenceBlock and crash in OwnerPtr::invalidate
            if(shouldDeleteReferenceBlock)
            {
                if(!mReferenceBlock->isReferenced() && !mReferenceBlock->isWeakReferenced())
                {
                    Memory::deleteObject(mReferenceBlock);
                }
            }
        }
        set(nullptr, nullptr);
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

class EnableWeakPtrToThis: public IPointedObject
{
template<class U>
friend class CountedPtrBase;

public:
    virtual ~EnableWeakPtrToThis() override
    {
        // INFO: This will invalidate the WeakPtr without removing mReferenceBlock, the block will be removed by the parent OwnerPtr!
        mPtrToThis.internalInvalidate(false);
    };
protected:
    template<class OtherClass>
    WeakPtr<OtherClass> getPtrToThis() { return WeakPtr<OtherClass>::cast(mPtrToThis); }
    template<class OtherClass>
    WeakPtr<const OtherClass> getPtrToThis() const { return WeakPtr<const OtherClass>::cast(mPtrToThis); }
private:
    template <class OtherClass>
    void set(const WeakPtr<OtherClass>& ptr) { mPtrToThis = WeakPtr<IPointedObject>(dynamic_cast<IPointedObject*>(const_cast<REMOVE_CONST(OtherClass)*>(ptr.getInternalPointer())), ptr.getReferenceBlock()); CHECK_MSG(mPtrToThis, "Invalid PtrToThis");  }
    WeakPtr<IPointedObject> mPtrToThis {};
};

// REF COUNTED PTR BASE
template<class T>
class CountedPtrBase : public BasePtr
{
template<class U>
friend class WeakPtr;
template<class U>
friend class Ptr;

public:
    virtual ~CountedPtrBase() { invalidate(); }
    operator WeakPtr<const T>() const { return WeakPtr<const T>(dynamic_cast<const T*>(mInternalPointer), mReferenceBlock); }
    T& get() const { return *mInternalPointer; }
    T* operator->() const { CHECK_MSG(this->isValid(), "Invalid pointer!"); return &get(); }
    bool isValid() const { return mReferenceBlock != nullptr && mReferenceBlock->isReferenced() && mInternalPointer != nullptr; }
    operator bool() const { return this->isValid(); }
    bool operator==(const WeakPtr<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    bool operator==(const CountedPtrBase<T>& otherRef) const { return this->mInternalPointer == otherRef.mInternalPointer; }
    void invalidate()
    {
        if(mReferenceBlock)
        {
            CHECK_MSG(mReferenceBlock->isReferenced(), "Weak references are already 0!")
            decrement();
            
            if(mInternalPointer && !mReferenceBlock->isReferenced())
            {
                // INFO: if class is EnableWeakPtrToThis derived, the mReferenceBlock will be removed by the parent OwnerPtr in the next if statement!
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
            if constexpr (IS_BASE_OF(EnableWeakPtrToThis, T))
            {
                EnableWeakPtrToThis* enableWeakPtrToThis = dynamic_cast<EnableWeakPtrToThis*>(const_cast<REMOVE_CONST(T)*>(reference));
                if(enableWeakPtrToThis)
                {
                    enableWeakPtrToThis->set(WeakPtr<T>(*this));
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
class SharedPtr : public CountedPtrBase<T>
{
template<class U>
friend class WeakPtr;
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
    SharedPtr(const WeakPtr<T>& other) { assign(other); }
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
    void assign(const WeakPtr<T>& other)
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
class OwnerPtr : public BaseOwnerPtr, public CountedPtrBase<T>
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
    operator WeakPtr<T>() const { return WeakPtr<T>(dynamic_cast<T*>(this->mInternalPointer), this->mReferenceBlock); }
    template<class U> T_EXTENDS(T, U) 
    operator WeakPtr<U>() const { return WeakPtr<U>(dynamic_cast<U*>(this->mInternalPointer), this->mReferenceBlock); }
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
NS_END

// HASH
// Needed for unordered_map
namespace std
{
  template<class T>
  struct hash<Core::Ptr<T>> 
  {
    size_t operator()(Core::Ptr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
  
  template<class T>
  struct hash<Core::WeakPtr<T>> 
  {
    size_t operator()(Core::WeakPtr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
  
  template<class T>
  struct hash<Core::CountedPtrBase<T>> 
  {
    size_t operator()(Core::CountedPtrBase<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };

  template<class T>
  struct hash<Core::SharedPtr<T>> 
  {
    size_t operator()(Core::SharedPtr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };

  template<class T>
  struct hash<Core::OwnerPtr<T>> 
  {
    size_t operator()(Core::OwnerPtr<T> const& pointer) const 
    {
      return size_t(&pointer.get());
    }
  };
}