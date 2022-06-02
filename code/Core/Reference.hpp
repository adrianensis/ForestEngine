#ifndef REFERENCE_HPP
#define REFERENCE_HPP

#include "Core/Memory.hpp"

// template <template <typename > class C, typename T>
// struct get_template_type<C<T>>
// {
//     using type = T;
// };

class BasePtr
{

};

template<class T>
class Ptr;

template<class T>
class OwnerPtr : public BasePtr
{
friend Ptr<T>;

public:

    template <class OtherClass>
    static OwnerPtr<T> Cast(const OwnerPtr<OtherClass>& other)
    {
        return OwnerPtr<T>(std::dynamic_pointer_cast<T>(other.getSharedPtr()));
    }

    OwnerPtr() = default;
    OwnerPtr(const OwnerPtr<T>& other) { setReference(other.mReference); }
    OwnerPtr(T* reference) { setReference(std::shared_ptr<T>(reference, OwnerPtrCustomDeleter())); }
    OwnerPtr(const std::weak_ptr<T> weakPtr) { setReference(std::shared_ptr<T>(weakPtr)); }

    operator OwnerPtr<const T>() const
    {
        return OwnerPtr<const T>( static_cast<std::shared_ptr<const T>>(mReference));
    }

    // HACK to get raw ptr reference, TODO : remove/refactor/limit
    const T& get() const { return *mReference.get(); }
    T& get() { return *mReference.get(); }
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

private:

    // Custom Deleter
    struct OwnerPtrCustomDeleter
    {
        // TODO : Debug WHY a nullptr is reaching this
        void operator()(T* p) const { if(p != nullptr) { DELETE(p);} }
    };

    void setReference(const std::shared_ptr<T> reference) { mReference = reference; }


private:
	std::shared_ptr<T> mReference;
};

template<class T>
class Ptr : public BasePtr
{
private:
    void setReference(std::weak_ptr<T> reference) { mReference = reference; }

public:

    static Ptr<const T> toConst()
    {
        return Ptr<const T>();
    }

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
    const T& get() const { return *mReference.lock().get(); }
    T& get() { return *mReference.lock().get(); }
    bool isValid() const { return !mReference.expired(); }

    Ptr<T>& operator=(const Ptr<T>& rhs)
	{
        setReference(rhs.mReference);
		return *this;
	}

    bool operator==(const Ptr<T>& otherRef) const
	{
		return this->mReference.lock().get() == otherRef.mReference.lock().get();
	}

    operator bool() const
    {
        return this->isValid();
    }

    std::weak_ptr<T> getWeakPtr() const { return mReference; }

private:
	std::weak_ptr<T> mReference;
};

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
    using type = OwnerPtr<const T>;
};

#endif