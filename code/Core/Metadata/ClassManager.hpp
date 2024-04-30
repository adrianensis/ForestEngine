#pragma once

#include "Core/StdCore.hpp"
#include "Core/Metadata/MetadataMacros.hpp"
#include "Core/ConstString/ConstString.hpp"
#include <unordered_map>
#include "Core/Assert/Assert.hpp"

#define REGISTER_CLASS(...) \
inline static const ClassDefinition smClassDefinition_##__VA_ARGS__ {#__VA_ARGS__##sv, sizeof(__VA_ARGS__)}; \
inline static const ClassRegister classRegister_##__VA_ARGS__ = ClassRegister(smClassDefinition_##__VA_ARGS__); \
template<> \
inline const ClassMetadata& ClassManager::getClassMetadata<__VA_ARGS__>() { return ClassManager::getClassMetadataById(smClassDefinition_##__VA_ARGS__.getId()); } \
template<> \
inline const ClassMetadata& ClassManager::getClassMetadataNoAssert<__VA_ARGS__>() { return getClassMetadata<__VA_ARGS__>(); }

#define REGISTER_MEMBER(memberName, ...) \
    inline static const MemberDefinition smMemberDefinition_##memberName {#memberName##sv, #__VA_ARGS__##sv, offsetof(ThisClass, memberName)}; \
    inline static const MemberRegister memberRegister_##memberName = MemberRegister(ClassManager::getClassMetadata<ThisClass>().mClassDefinition.mName, smMemberDefinition_##memberName);

class ClassDefinition
{
public:
    ClassId getId() const { return mName.getHash(); };
public:
    ConstString mName;
    u32 mTypeSize = 0;
};

class MemberDefinition
{
public:
    ConstString mName;
    ConstString mClassName;
    u32 mOffset = 0;
};

// --------------------------------------------------------
// MEMBER
// --------------------------------------------------------

class MemberRegister
{
public:
    MemberRegister(const ConstString& ownerClassName, const MemberDefinition& memberDefinition);
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
};

class ClassMetadata
{
public:
    ClassMetadata(const ClassDefinition& classDefinition);

    ClassDefinition mClassDefinition;
    std::unordered_map<ConstString, MemberMetadata> mMembersMap;
};

// --------------------------------------------------------
// CLASS MANAGER
// --------------------------------------------------------

class ClassManager
{
friend ClassRegister;
friend MemberRegister;

public:
    static const ClassMetadata& getClassMetadataByName(const ConstString& className);
    static const ClassMetadata& getClassMetadataById(const ClassId classId);

    inline static const ClassDefinition smNullClassDefinition = ClassDefinition();
    inline static const ClassMetadata smNullClassMetadata = ClassMetadata(smNullClassDefinition);
    template<class T>
    static const ClassMetadata& getClassMetadata()
    {
        CHECK_MSG(false, "getClassDefinition not specialized!");
        return smNullClassMetadata;
    }

    template<class T>
    static const ClassMetadata& getClassMetadataNoAssert()
    {
        return smNullClassMetadata;
    }

    template<class T>
    static const ClassMetadata& getDynamicClassMetadata(const T* pointer)
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
    template<class T>
    static void unregisterDynamicClass(const T* pointer)
    {
        unregisterDynamicClass(reinterpret_cast<u64>(pointer));
    }
private:
    static void insert(const ClassMetadata& classMetadata);
    static ClassMetadata& getClassMetadataInternal(const ConstString& className);
    static void registerDynamicClass(u64 pointer, ClassId classId);
    static void unregisterDynamicClass(u64 pointer);
    inline static std::unordered_map<ConstString, ClassMetadata> smClassMapByName;
    inline static std::unordered_map<ClassId, ClassMetadata*> smClassMapById;
    inline static std::unordered_map<u64, ClassMetadata*> smPointersToDynamicClass;
};