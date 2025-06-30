#pragma once

#include "Core/StdCore.hpp"
#include "Core/Metadata/MetadataMacros.hpp"
#include "Core/HashedString/HashedString.hpp"
#include <unordered_map>
#include <typeinfo>

using ClassId = u64;
using InternalCPPTypeId = u64;

#define REGISTER_CLASS(...) \
inline static const ClassDefinition smClassDefinition_##__VA_ARGS__ {#__VA_ARGS__##sv, sizeof(__VA_ARGS__)}; \
inline static const ClassRegisterHelper classRegisterHelper_##__VA_ARGS__ = ClassRegisterHelper(typeid(__VA_ARGS__).hash_code(), smClassDefinition_##__VA_ARGS__);

#define REGISTER_MEMBER(memberName, ...) \
    inline static const MemberDefinition smMemberDefinition_##memberName {#memberName##sv, #__VA_ARGS__##sv, offsetof(ThisClass, memberName)}; \
    inline static const MemberRegister memberRegister_##memberName = MemberRegister(ClassManager::getClassMetadata<ThisClass>().mClassDefinition.mName, smMemberDefinition_##memberName);

class ClassDefinition
{
public:
    ClassId getId() const { return mName.getHash(); };
public:
    HashedString mName;
    u32 mTypeSize = 0;
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

        return smClassMapById.at(smInternalCPPTypeIdToClassId.at(internalCPPTypeId));
    }

    template<class T>
    static const ClassMetadata& getClassMetadataNoAssert()
    {
        InternalCPPTypeId internalCPPTypeId = typeid(T).hash_code();
        
        if(! smInternalCPPTypeIdToClassId.contains(internalCPPTypeId))
        {
            return smNullClassMetadata;
        }

        return smClassMapById.at(smInternalCPPTypeIdToClassId.at(internalCPPTypeId));
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
    inline static std::unordered_map<InternalCPPTypeId, ClassId> smInternalCPPTypeIdToClassId;
    inline static std::unordered_map<ClassId, ClassMetadata> smClassMapById;
    inline static std::unordered_map<u64, ClassMetadata*> smPointersToDynamicClass;
};