#pragma once

#include "Core/Memory.hpp"

class ObjectBase;

using ClassRegisterCallback = std::function<ObjectBase*()>;
using MemberRegisterCallback = std::function<ClassId()>;

#define REGISTER_CLASS(...) \
    inline static const ClassRegister classRegister_##__VA_ARGS__ {__VA_ARGS__::getClassNameStatic(), [](){ \
        return Memory::newObject<__VA_ARGS__>(); \
    }};

#define REGISTER_MEMBER(...) \
    inline static const MemberRegister memberRegister_##__VA_ARGS__ = MemberRegister(ThisClass::getClassNameStatic(), #__VA_ARGS__, [](){ \
        return decltype(__VA_ARGS__)::getClassIdStatic(); \
    });

// --------------------------------------------------------
// MEMBER
// --------------------------------------------------------

class MemberRegister
{
public:
    MemberRegister(const std::string_view& ownerClassName,const std::string_view& name, const MemberRegisterCallback& callback);
};

class MemberInfo
{
public:
    MemberInfo(const std::string_view& name, const MemberRegisterCallback& callback);
    std::string_view mName;
    // TODO: add ClassId
    MemberRegisterCallback mCallback;
};

// --------------------------------------------------------
// CLASS
// --------------------------------------------------------

class ClassRegister
{
public:
    ClassRegister(const std::string_view& className);
    ClassRegister(const std::string_view& className, const ClassRegisterCallback& callback);
};

class ClassInfo
{
public:
    ClassInfo(const std::string_view& className);
    ClassInfo(const std::string_view& className, const ClassRegisterCallback& callback);

    std::string_view mClassName;
    // TODO: add ClassId
    ClassRegisterCallback mCallback;
    std::unordered_map<std::string_view, MemberInfo> mMembersMap;
};

// --------------------------------------------------------
// CLASS MANAGER
// --------------------------------------------------------

class ClassManager
{
friend ClassRegister;
friend MemberRegister;

public:
    template<typename T>
    static T* instance(const std::string_view& className)
    {
        return static_cast<T*>(instance(className));
    }

    static ObjectBase* instance(const std::string_view& className);

    static const ClassInfo& getClassInfo(const std::string_view& className);

private:
    static ClassInfo& getOrCreate(const std::string_view& className);
    // TODO: add other map, sort classes by ID
    inline static std::unordered_map<std::string_view, ClassInfo> mClassMap;
};