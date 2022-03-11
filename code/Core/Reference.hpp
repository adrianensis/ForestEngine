#pragma once

#include "Core/Memory.hpp"

template<class T>
class Ptr;

template<class T>
class OwnerPtr
{
friend Ptr<T>;
private:

    // Custom Deleter
    struct OwnerPtrCustomDeleter
    {
        // TODO : Debug WHY a nullptr is reaching this
        void operator()(T* p) const { if(p != nullptr) { DELETE(p);} }
    };

	std::shared_ptr<T> mReference;

    void setReference(const std::shared_ptr<T> reference) { mReference = reference; }

    OwnerPtr(const std::weak_ptr<T> weakPtr) { setReference(std::shared_ptr<T>(weakPtr)); }

public:
    
    template <class OtherClass>
    static OwnerPtr<T> Cast(const OwnerPtr<OtherClass>& other)
    {
        return OwnerPtr<T>(std::dynamic_pointer_cast<T>(other.getSharedPtr()));
    }

    OwnerPtr() = default;
    OwnerPtr(const OwnerPtr<T>& other) { setReference(other.mReference); }
    OwnerPtr(T* reference) { setReference(std::shared_ptr<T>(reference, OwnerPtrCustomDeleter())); }

    operator OwnerPtr<const T>() const
    {
        return OwnerPtr<const T>( static_cast<std::shared_ptr<const T>>(mReference));
    }

    // HACK to get raw ptr reference, TODO : remove/refactor/limit
    T& get() const { return *mReference.get(); }
    bool isValid() const { return mReference != nullptr; }

    OwnerPtr<T>& operator=(const OwnerPtr<T>& rhs)
	{
        setReference(rhs.mReference);
		return *this;
	}

    operator bool() const
    {
        return this->isValid();
    }

    bool operator==(const OwnerPtr<T>& otherRef) const
	{
		return this->mReference == otherRef.mReference;
	}

    bool operator!=(const OwnerPtr<T>& otherRef) const
	{
		return (*this == otherRef);
	}

    std::shared_ptr<T> getSharedPtr() const { return mReference; }
};

template<class T>
class Ptr
{
private:
	std::weak_ptr<T> mReference;

    void setReference(std::weak_ptr<T> reference) { mReference = reference; }
public:

    template <class OtherClass>
    static Ptr<T> Cast(const Ptr<OtherClass>& other)
    {
        return Ptr<T>(std::dynamic_pointer_cast<T>(other.getWeakPtr().lock()));
    }

    template <class OtherClass>
    static Ptr<T> Cast(const OwnerPtr<OtherClass>& other)
    {
        return Ptr<T>(std::dynamic_pointer_cast<T>(other.getSharedPtr()));
    }

    Ptr() = default;
    Ptr(const Ptr<T>& other) { setReference(other.mReference); }
    Ptr(const std::weak_ptr<T> weakPtr) { setReference(weakPtr); }
    Ptr(const std::shared_ptr<T> sharedPtr) { setReference(std::weak_ptr<T>(sharedPtr)); }
    Ptr(const OwnerPtr<T>& owner) { setReference(std::weak_ptr<T>(owner.mReference)); }

    operator Ptr<const T>() const
    {
        return Ptr<const T>(static_cast<std::weak_ptr<const T>>(mReference));
    }

    operator OwnerPtr<T>() const
    {
        return OwnerPtr<T>(mReference);
    }

    // HACK to get raw ptr reference, TODO : remove/refactor/limit
    T& get() const { return *mReference.lock().get(); }
    bool isValid() const { return !mReference.expired(); }

    Ptr<T>& operator=(const Ptr<T>& rhs)
	{
        setReference(rhs.mReference);
		return *this;
	}

    operator bool() const
    {
        return this->isValid();
    }

    std::weak_ptr<T> getWeakPtr() const { return mReference; }
};