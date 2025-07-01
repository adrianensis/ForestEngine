#pragma once

#include "Core/StdCore.hpp"
#include "Core/Metadata/MetadataMacros.hpp"
#include "Core/HashedString/HashedString.hpp"
#include <unordered_map>
#include <typeinfo>

using ClassId = Core::u64;
using InternalCPPTypeId = Core::u64;

NS_BEGIN(Core)
#define REGISTER_CLASS(...) \
    inline static const Core::ClassDefinition smClassDefinition_##__VA_ARGS__ {#__VA_ARGS__##sv, sizeof(__VA_ARGS__)}; \
    inline static const Core::ClassRegisterHelper classRegisterHelper_##__VA_ARGS__ = Core::ClassRegisterHelper(typeid(__VA_ARGS__).hash_code(), smClassDefinition_##__VA_ARGS__);

#define REGISTER_MEMBER(memberName, ...) \
    inline static const Core::MemberDefinition smMemberDefinition_##memberName {#memberName##sv, #__VA_ARGS__##sv, offsetof(ThisClass, memberName)}; \
    inline static const Core::MemberRegister memberRegister_##memberName = Core::MemberRegister(Core::ClassManager::getClassMetadata<ThisClass>().mClassDefinition.mName, smMemberDefinition_##memberName);

class ClassDefinition
{
public:
    ClassId getId() const { return mName.getHash(); };
public:
    Core::HashedString mName;
    Core::u32 mTypeSize = 0;
};

class MemberDefinition
{
public:
    Core::HashedString mName;
    Core::HashedString mClassName;
    Core::u32 mOffset = 0;
};

// --------------------------------------------------------
// MEMBER
// --------------------------------------------------------

class MemberRegister
{
public:
    MemberRegister(const Core::HashedString& ownerClassName, const MemberDefinition& memberDefinition);
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
    std::unordered_map<Core::HashedString, MemberMetadata> mMembersMap;
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
        if(smPointersToDynamicClass.contains(reinterpret_cast<Core::u64>(pointer)))
        {
            return *smPointersToDynamicClass.at(reinterpret_cast<Core::u64>(pointer));
        }

        return smNullClassMetadata;
    }

    template<class T>
    static void registerDynamicClass(const T* pointer)
    {
        registerDynamicClass(reinterpret_cast<Core::u64>(pointer), ClassManager::getClassMetadataNoAssert<T>().mClassDefinition.getId());
    }
    static void unregisterDynamicClass(const void* pointer)
    {
        unregisterDynamicClass(reinterpret_cast<Core::u64>(pointer));
    }
private:
    static void insert(InternalCPPTypeId internalCPPId, const ClassMetadata& classMetadata);
    static ClassMetadata& getClassMetadataByIdInternal(const ClassId classId);
    static void registerDynamicClass(Core::u64 pointer, ClassId classId);
    static void unregisterDynamicClass(Core::u64 pointer);
    inline static std::unordered_map<InternalCPPTypeId, ClassId> smInternalCPPTypeIdToClassId;
    inline static std::unordered_map<ClassId, ClassMetadata> smClassMapById;
    inline static std::unordered_map<Core::u64, ClassMetadata*> smPointersToDynamicClass;
};
NS_END