#pragma once

#include "Core/Memory.hpp"

template<class T>
class RefRaw
{
private:
	T* mReference = nullptr;

    void setReference(T* reference) { mReference = reference; }
public:
    RefRaw() = default;
    RefRaw(const RefRaw<T>& other) { setReference(other.mReference); }
    RefRaw(T* reference) { setReference(reference); }
    RefRaw(const std::shared_ptr<T>& sharedPtr) { setReference(sharedPtr.get()); }

    operator RefRaw<const T>() const
    {
        return RefRaw<const T>( static_cast<const T*>(mReference));
    }

    T& get() const { return *mReference; }
    bool isValid() const { return mReference != nullptr; }

    RefRaw& operator=(const RefRaw<T>& rhs)
	{
        setReference(rhs.mReference);
		return *this;
	}

    RefRaw& operator=(T* rhs)
	{
        setReference(rhs);
		return *this;
	}

    operator bool() const
    {
        return this->isValid();
    }
};

template<class T>
class Ref;

template<class T>
class OwnerRef
{
friend Ref<T>;
private:

    // Custom Deleter
    struct OwnerRefCustomDeleter
    {
        void operator()(T* p) const { DELETE(p); }
    };

	std::shared_ptr<T> mReference;

    void setReference(const std::shared_ptr<T>& reference) { mReference = reference; }
public:
    OwnerRef() = default;
    OwnerRef(const OwnerRef<T>& other) { setReference(other.mReference); }
    OwnerRef(T* reference) { setReference(std::shared_ptr<T>(reference, OwnerRefCustomDeleter())); }

    operator OwnerRef<const T>() const
    {
        return OwnerRef<const T>( static_cast<std::shared_ptr<const T>>(mReference));
    }

    // HACK to get raw ptr reference, TODO : remove/refactor/limit
    T& get() const { return *mReference.get(); }
    bool isValid() const { return mReference != nullptr; }

    OwnerRef& operator=(const OwnerRef<T>& rhs)
	{
        setReference(rhs.mReference);
		return *this;
	}

    operator bool() const
    {
        return this->isValid();
    }

    bool operator==(const OwnerRef<T>& otherRef) const
	{
		return this->mReference == otherRef.mReference;
	}

    bool operator!=(const OwnerRef<T>& otherRef) const
	{
		return (*this == otherRef);
	}
};

template<class T>
class Ref
{
private:
	std::weak_ptr<T> mReference;

    void setReference(std::weak_ptr<T> reference) { mReference = reference; }
public:
    Ref() = default;
    Ref(const Ref<T>& other) { setReference(other.mReference); }
    Ref(const std::weak_ptr<T>& weakPtr) { setReference(weakPtr); }
    Ref(const std::shared_ptr<T>& sharedPtr) { setReference(std::weak_ptr<T>(sharedPtr)); }
    Ref(const OwnerRef<T>& owner) { setReference(std::weak_ptr<T>(owner.mReference)); }

    operator Ref<const T>() const
    {
        return Ref<const T>( static_cast<std::weak_ptr<const T>>(mReference));
    }

    // HACK to get raw ptr reference, TODO : remove/refactor/limit
    T& get() const { return *mReference.lock().get(); }
    bool isValid() const { return !mReference.expired(); }

    Ref& operator=(const Ref<T>& rhs)
	{
        setReference(rhs.mReference);
		return *this;
	}

    operator bool() const
    {
        return this->isValid();
    }
};