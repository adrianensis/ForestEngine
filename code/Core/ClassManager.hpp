#pragma once

#include "Core/Memory.hpp"

#define REGISTER_CLASS(...) \
    inline static const ClassRegister classRegister_##__VA_ARGS__ = ClassRegister(__VA_ARGS__::getClassNameStatic(), __VA_ARGS__::getClassIdStatic(), [](){ \
        return Memory::newObject<__VA_ARGS__>(); \
    });

#define REGISTER_MEMBER(...) \
    inline static const MemberRegister memberRegister_##__VA_ARGS__ = MemberRegister(ThisClass::getClassNameStatic(), #__VA_ARGS__, decltype(__VA_ARGS__)::getClassNameStatic(), decltype(__VA_ARGS__)::getClassIdStatic());

class ObjectBase;

using ClassRegisterCallback = std::function<ObjectBase*()>;

// --------------------------------------------------------
// MEMBER
// --------------------------------------------------------

class MemberRegister
{
public:
    MemberRegister(const std::string_view& ownerClassName, const std::string_view& name, const std::string_view& className, ClassId classId);
};

class MemberInfo
{
public:
    MemberInfo(const std::string_view& name, const std::string_view& className, ClassId classId);
    std::string_view mName;
    std::string_view mClassName;
    ClassId mClassId;
};

// --------------------------------------------------------
// CLASS
// --------------------------------------------------------

class ClassRegister
{
public:
    ClassRegister(const std::string_view& className, ClassId classId);
    ClassRegister(const std::string_view& className, ClassId classId, const ClassRegisterCallback& callback);
};

class ClassInfo
{
public:
    ClassInfo(const std::string_view& className, ClassId classId);
    ClassInfo(const std::string_view& className, ClassId classId, const ClassRegisterCallback& callback);

    std::string_view mClassName;
    ClassId mClassId;
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
    static ClassInfo& getClassInfoInternal(const std::string_view& className);
    static ClassInfo& getOrCreate(const std::string_view& className, ClassId classId);
    // TODO: add other map, sort classes by ID
    inline static std::unordered_map<std::string_view, ClassInfo> mClassMapByName;
    inline static std::unordered_map<ClassId, ClassInfo*> mClassMapById;
};