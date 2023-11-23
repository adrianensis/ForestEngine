#pragma once

#include "Core/Std.hpp"

#define REGISTER_CLASS(...) \
    inline static const ClassRegister classRegister_##__VA_ARGS__ = ClassRegister(__VA_ARGS__::getClassNameStatic(), __VA_ARGS__::getClassIdStatic(), [](){ \
        return Memory::newObject<__VA_ARGS__>(); \
    });

#define REGISTER_MEMBER(...) \
    inline static const MemberRegister memberRegister_##__VA_ARGS__ = MemberRegister(ThisClass::getClassNameStatic(), #__VA_ARGS__, decltype(__VA_ARGS__)::getClassNameStatic(), decltype(__VA_ARGS__)::getClassIdStatic());

/*
    NOTE: tagging methods as virtual here have consecuences: "virtual ClassId getClassId()"
    may cause "struct"-like classes to have a VTable, which increases the struct size.
    For example: Vector3 is sizeof(f32) * 3 = 4*3 = 12, but with VTable is goes up to 24!!
    Engine heavily depends on the exact size of a Vector3 (and other classes).
*/
#define DECLARE_METADATA_METHODS(Virtual, Override) \
	constexpr static ClassId getClassIdStatic() { return smClassId; }; \
    constexpr static const std::string_view& getClassNameStatic() { return smClassName; }; \
	constexpr Virtual ClassId getClassId() const Override { return ThisClass::getClassIdStatic(); }; \
	constexpr Virtual const std::string_view& getClassName() const Override { return ThisClass::getClassNameStatic(); };

#define DECLARE_METADATA_VARIABLES(...)                                    \
    private:                                                               \
        using ThisClass = __VA_ARGS__;                                     \
        constexpr inline static const std::string_view smClassName = #__VA_ARGS__##sv; \
        constexpr inline static const ClassId smClassId = Hash::hashString(ThisClass::smClassName); 

#define GENERATE_METADATA(...)                      \
    private: \
        DECLARE_METADATA_VARIABLES(__VA_ARGS__)             \
    public:                                             \
        DECLARE_METADATA_METHODS(virtual, override) \
    private:                                            \
        DECLARE_GET_PTR_THIS()               \
        REGISTER_CLASS(__VA_ARGS__) \
    private: // NOTE: notice the last blank space " "

#define GENERATE_METADATA_STRUCT(...)              \
        DECLARE_METADATA_VARIABLES(__VA_ARGS__)            \
    public:                                            \
        DECLARE_METADATA_METHODS(EMPTY_MACRO(), EMPTY_MACRO()) \
    private: // NOTE: notice the last blank space " "


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

    static const ClassInfo& getClassInfoByClassName(const std::string_view& className);
    static const ClassInfo& getClassInfoByClassId(const ClassId classId);

private:
    static ClassInfo& insert(const std::string_view& className, ClassId classId);
    static ClassInfo& getClassInfoInternal(const std::string_view& className);
    inline static std::unordered_map<std::string_view, ClassInfo> mClassMapByName;
    inline static std::unordered_map<ClassId, ClassInfo*> mClassMapById;
};