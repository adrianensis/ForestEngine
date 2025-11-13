#pragma once

#include "Core/CoreBase.hpp"
#include "Core/Metadata/MetadataTypes.hpp"
#include "Core/HashedString/HashedString.hpp"
#include <unordered_map>
#include <typeinfo>


NS_BEGIN(Core)

// Class, Bases...
#define REGISTER_CLASS(Class, ...) \
    inline static const Core::ClassDefinition smClassDefinition_##Class(#Class##sv, sizeof(Class) __VA_OPT__(,) __VA_OPT__({) FOR_EACH(TO_STRING_AND_ADD_TRAIL_COMMA, __VA_ARGS__) __VA_OPT__(})); \
    inline static const Core::ClassRegisterHelper classRegisterHelper_##Class = Core::ClassRegisterHelper(typeid(Class).hash_code(), smClassDefinition_##Class);

#define REGISTER_MEMBER(memberName, ...) \
    inline static const Core::MemberDefinition smMemberDefinition_##memberName {#memberName##sv, #__VA_ARGS__##sv, offsetof(ThisClass, memberName)}; \
    inline static const Core::MemberRegister memberRegister_##memberName = Core::MemberRegister(Core::ClassManager::getClassMetadata<ThisClass>().mClassDefinition.mName, smMemberDefinition_##memberName);

#define P(...)          \
    __VA_ARGS__;        \
    REGISTER_MEMBER(__VA_ARGS__);

#define MAX_CLASS_BASES 32

class ClassDefinition
{
public:
    ClassDefinition() = default;
    ClassDefinition(HashedString name, u32 typeSize);
    ClassDefinition(HashedString name, u32 typeSize, const std::array<HashedString, MAX_CLASS_BASES>& bases);
    ClassId getId() const;
    bool isA(ClassId classId) const;

public:
    HashedString mName;
    u32 mTypeSize = 0;
    std::array<ClassId, MAX_CLASS_BASES> mBases;
};

class MemberDefinition
{
public:
    HashedString mName;
    HashedString mClassName;
    u32 mOffset = 0;
};

// --------------------------------------------------------
// MEMBER
// --------------------------------------------------------

class MemberRegister
{
public:
    MemberRegister(const HashedString& ownerClassName, const MemberDefinition& memberDefinition);
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

class ClassRegisterHelper
{
public:
    ClassRegisterHelper(InternalCPPTypeId internalCPPId, const ClassDefinition& classDefinition);
};

class ClassMetadata
{
public:
    ClassMetadata(const ClassDefinition& classDefinition);

    ClassDefinition mClassDefinition;
    std::unordered_map<HashedString, MemberMetadata> mMembersMap;
};

// --------------------------------------------------------
// CLASS MANAGER
// --------------------------------------------------------

class ClassManager
{
friend ClassRegisterHelper;
friend MemberRegister;

public:
    static const ClassMetadata& getClassMetadataById(const ClassId classId);

    inline static const ClassDefinition smNullClassDefinition = ClassDefinition();
    inline static const ClassMetadata smNullClassMetadata = ClassMetadata(smNullClassDefinition);
    template<class T>
    static const ClassMetadata& getClassMetadata()
    {
        InternalCPPTypeId internalCPPTypeId = typeid(T).hash_code();
        
        if(! smInternalCPPTypeIdToClassId.contains(internalCPPTypeId))
        {
            CHECK_MSG(false, "getClassDefinition not specialized!");
        }

        return *smInternalCPPTypeIdToClassId.at(internalCPPTypeId);
    }

    template<class T>
    static const ClassMetadata& getClassMetadataNoAssert()
    {
        InternalCPPTypeId internalCPPTypeId = typeid(T).hash_code();
        
        if(! smInternalCPPTypeIdToClassId.contains(internalCPPTypeId))
        {
            return smNullClassMetadata;
        }

        return *smInternalCPPTypeIdToClassId.at(internalCPPTypeId);
    }

    static const ClassMetadata& getDynamicClassMetadata(const void* pointer)
    {
        if(smPointersToDynamicClass.contains(reinterpret_cast<u64>(pointer)))
        {
            return *smPointersToDynamicClass.at(reinterpret_cast<u64>(pointer));
        }

        return smNullClassMetadata;
    }

    template<class T>
    static void registerDynamicClass(const T* pointer)
    {
        registerDynamicClass(reinterpret_cast<u64>(pointer), ClassManager::getClassMetadataNoAssert<T>().mClassDefinition.getId());
    }
    static void unregisterDynamicClass(const void* pointer)
    {
        unregisterDynamicClass(reinterpret_cast<u64>(pointer));
    }
private:
    static void insert(InternalCPPTypeId internalCPPId, const ClassMetadata& classMetadata);
    static ClassMetadata& getClassMetadataByIdInternal(const ClassId classId);
    static void registerDynamicClass(u64 pointer, ClassId classId);
    static void unregisterDynamicClass(u64 pointer);
    inline static std::unordered_map<InternalCPPTypeId, ClassMetadata*> smInternalCPPTypeIdToClassId;
    inline static std::unordered_map<ClassId, ClassMetadata> smClassMapById;
    inline static std::unordered_map<u64, ClassMetadata*> smPointersToDynamicClass;
};
NS_END