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
        // TODO : Debug WHY a nullptr is reaching this
        void operator()(T* p) const { if(p != nullptr) { DELETE(p);} }
    };

	std::shared_ptr<T> mReference;

    void setReference(const std::shared_ptr<T> reference) { mReference = reference; }

    OwnerRef(const std::weak_ptr<T> weakPtr) { setReference(std::shared_ptr<T>(weakPtr)); }

public:
    
    template <class OtherClass>
    static OwnerRef<T> Cast(const OwnerRef<OtherClass>& other)
    {
        return OwnerRef<T>(std::dynamic_pointer_cast<T>(other.getSharedPtr()));
    }

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

    OwnerRef<T>& operator=(const OwnerRef<T>& rhs)
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

    std::shared_ptr<T> getSharedPtr() const { return mReference; }
};

template<class T>
class Ref
{
private:
	std::weak_ptr<T> mReference;

    void setReference(std::weak_ptr<T> reference) { mReference = reference; }
public:

    template <class OtherClass>
    static Ref<T> Cast(const Ref<OtherClass>& other)
    {
        return Ref<T>(std::dynamic_pointer_cast<T>(other.getWeakPtr().lock()));
    }

    template <class OtherClass>
    static Ref<T> Cast(const OwnerRef<OtherClass>& other)
    {
        return Ref<T>(std::dynamic_pointer_cast<T>(other.getSharedPtr()));
    }

    Ref() = default;
    Ref(const Ref<T>& other) { setReference(other.mReference); }
    Ref(const std::weak_ptr<T> weakPtr) { setReference(weakPtr); }
    Ref(const std::shared_ptr<T> sharedPtr) { setReference(std::weak_ptr<T>(sharedPtr)); }
    Ref(const OwnerRef<T>& owner) { setReference(std::weak_ptr<T>(owner.mReference)); }

    operator Ref<const T>() const
    {
        return Ref<const T>(static_cast<std::weak_ptr<const T>>(mReference));
    }

    operator OwnerRef<T>() const
    {
        return OwnerRef<T>(mReference);
    }

    // HACK to get raw ptr reference, TODO : remove/refactor/limit
    T& get() const { return *mReference.lock().get(); }
    bool isValid() const { return !mReference.expired(); }

    Ref<T>& operator=(const Ref<T>& rhs)
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