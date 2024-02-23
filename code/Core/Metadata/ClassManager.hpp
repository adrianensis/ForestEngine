#pragma once

#include "Core/StdCore.hpp"
#include "Core/Metadata/MetadataMacros.hpp"
#include <unordered_map>
#include <functional>

#define REGISTER_CLASS(...) \
    inline static const ClassRegister classRegister = ClassRegister(smClassDefinition, [](){ \
        return Memory::newObject<__VA_ARGS__>(); \
    });

#define REGISTER_MEMBER(memberName, ...) \
    constexpr inline static const MemberDefinition smMemberDefinition_##memberName {#memberName##sv, #__VA_ARGS__##sv, offsetof(ThisClass, memberName)}; \
    inline static const MemberRegister memberRegister_##memberName = MemberRegister(ThisClass::getClassDefinitionStatic().mName, smMemberDefinition_##memberName);

/*
    NOTE: tagging methods as virtual here have consecuences: "virtual ClassId getClassDefinition().mId"
    may cause "struct"-like classes to have a VTable, which increases the struct size.
    For example: Vector3 is sizeof(f32) * 3 = 4*3 = 12, but with VTable is goes up to 24!!
    Engine heavily depends on the exact size of a Vector3 (and other classes).
*/
#define DECLARE_METADATA_VARIABLES(...)                                    \
    private:                                                               \
        using ThisClass = __VA_ARGS__;                                     \
        constexpr inline static const ClassDefinition smClassDefinition {#__VA_ARGS__##sv, Hash::hashString(#__VA_ARGS__##sv)};

#define DECLARE_METADATA_METHODS(Virtual, Override) \
	constexpr inline static const ClassDefinition& getClassDefinitionStatic() { return smClassDefinition; }; \
	Virtual const ClassDefinition& getClassDefinition() const Override { return getClassDefinitionStatic(); };

#define GENERATE_METADATA(...)                      \
    private: \
        DECLARE_METADATA_VARIABLES(__VA_ARGS__)             \
    public:                                             \
        DECLARE_METADATA_METHODS(virtual, override) \
    private: \
        REGISTER_CLASS(__VA_ARGS__) \
    private: // NOTE: notice the last blank space " "

#define GENERATE_METADATA_STRUCT(...)              \
        DECLARE_METADATA_VARIABLES(__VA_ARGS__)            \
    public:                                            \
        DECLARE_METADATA_METHODS(EMPTY_MACRO(), EMPTY_MACRO()) \
    private: \
        REGISTER_CLASS(__VA_ARGS__) \
    private: // NOTE: notice the last blank space " "


using ClassRegisterCallback = std::function<void*()>;

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
    std::string_view mClassName;
    u32 mOffset = 0;
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
    ClassRegister(const ClassDefinition& classDefinition, const ClassRegisterCallback& callback);
};

class ClassMetadata
{
public:
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

    static void* instance(const std::string_view& className);

    static const ClassMetadata& getClassMetadataByName(const std::string_view& className);
    static const ClassMetadata& getClassMetadataById(const ClassId classId);

private:
    static void insert(const ClassMetadata& classMetadata);
    static ClassMetadata& getClassMetadataInternal(const std::string_view& className);
    inline static std::unordered_map<std::string_view, ClassMetadata> mClassMapByName;
    inline static std::unordered_map<ClassId, ClassMetadata*> mClassMapById;
};