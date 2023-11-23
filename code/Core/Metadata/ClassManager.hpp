#pragma once

#include "Core/Std.hpp"

#define REGISTER_CLASS(...) \
    inline static const ClassRegister classRegister_##__VA_ARGS__ = ClassRegister(__VA_ARGS__::getClassDefinitionStatic(), [](){ \
        return Memory::newObject<__VA_ARGS__>(); \
    });

#define REGISTER_MEMBER(...) \
    inline static const MemberRegister memberRegister_##__VA_ARGS__ = MemberRegister(ThisClass::getClassDefinitionStatic().mName, #__VA_ARGS__, decltype(__VA_ARGS__)::getClassDefinitionStatic());

/*
    NOTE: tagging methods as virtual here have consecuences: "virtual ClassId getClassDefinition().mId"
    may cause "struct"-like classes to have a VTable, which increases the struct size.
    For example: Vector3 is sizeof(f32) * 3 = 4*3 = 12, but with VTable is goes up to 24!!
    Engine heavily depends on the exact size of a Vector3 (and other classes).
*/
#define DECLARE_METADATA_METHODS(Virtual, Override) \
	constexpr static const ClassDefinition& getClassDefinitionStatic() { return smClassDefinition; }; \
	constexpr Virtual const ClassDefinition& getClassDefinition() const Override { return getClassDefinitionStatic(); };

#define DECLARE_METADATA_VARIABLES(...)                                    \
    private:                                                               \
        using ThisClass = __VA_ARGS__;                                     \
        constexpr inline static const ClassDefinition smClassDefinition {#__VA_ARGS__##sv, Hash::hashString(#__VA_ARGS__##sv)};

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

class ClassDefinition
{
public:
    std::string_view mName;
    ClassId mId = 0;
};

class MemberDefinition
{
public:
    std::string_view mName;
    ClassDefinition mClassDefinition;
};

// --------------------------------------------------------
// MEMBER
// --------------------------------------------------------

class MemberRegister
{
public:
    MemberRegister(const std::string_view& ownerClassName, const MemberDefinition& memberDefinition);
};

class MemberMetadata
{
public:
    MemberMetadata(const MemberDefinition& memberDefinition);
    MemberDefinition mMemberDefinition;
};

// --------------------------------------------------------
// CLASS
// --------------------------------------------------------

class ClassRegister
{
public:
    ClassRegister(const ClassDefinition& classDefinition);
    ClassRegister(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback);
};

class ClassMetadata
{
public:
    ClassMetadata(const ClassDefinition& classDefinition);
    ClassMetadata(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback);

    ClassDefinition mClassDefinition;
    ClassRegisterCallback mCallback;
    std::unordered_map<std::string_view, MemberMetadata> mMembersMap;
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

    static const ClassMetadata& getClassMetadataByName(const std::string_view& className);
    static const ClassMetadata& getClassMetadataById(const ClassId classId);

private:
    static ClassMetadata& insert(const ClassDefinition& classDefinition);
    static ClassMetadata& getClassMetadataInternal(const std::string_view& className);
    inline static std::unordered_map<std::string_view, ClassMetadata> mClassMapByName;
    inline static std::unordered_map<ClassId, ClassMetadata*> mClassMapById;
};