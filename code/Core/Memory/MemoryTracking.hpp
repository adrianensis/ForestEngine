#pragma once

#include <unordered_map>
#include "Core/Assert/Assert.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/HashedString/HashedString.hpp"
#include "Core/Profiler/Profiler.hpp"

NS_BEGIN(Core)
class MemoryTracking
{
private:
#ifdef ENGINE_BUILD_DEBUG
	class AllocationInfo
    {
    public:
        AllocationInfo() = default;
        unsigned int mCurrentAllocations = 0;
        unsigned int mMaxAllocations = 0;
    };
#endif

public:
    static void init();

    static void terminate();
    static void log();
	
	template <class T>
	static void registerNewObject(const T* pointer)
	{
        PROFILER_CPU()
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");

#ifdef ENGINE_BUILD_DEBUG
		Core::HashedString className;

		if (Core::ClassManager::getClassMetadataNoAssert<T>().mClassDefinition.getId() > 0)
		{
			className = Core::ClassManager::getClassMetadata<T>().mClassDefinition.mName;
		}
		else
		{
			className = typeid(T).name();
		}

		if (!smAllocationsMap.contains(className))
        {
			smAllocationsMap.insert_or_assign(className, AllocationInfo());
		}

        smPointersToDynamicClassName.insert_or_assign(reinterpret_cast<u64>(pointer), className);

        smAllocationsMap[className].mCurrentAllocations += 1;
        smAllocationsMap[className].mMaxAllocations = std::max(smAllocationsMap[className].mCurrentAllocations, smAllocationsMap[className].mMaxAllocations);
#endif
	}

	template <class T>
	static void unregisterDeletedObject(const T* pointer)
	{
		
#ifdef ENGINE_BUILD_DEBUG
		PROFILER_CPU()
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");

		u64 ptrU64 = reinterpret_cast<u64>(pointer);
		if(!smPointersToDynamicClassName.contains(ptrU64))
		{
			return;
		}

		Core::HashedString className;
		if (Core::ClassManager::getDynamicClassMetadata(pointer).mClassDefinition.getId() > 0)
		{
			className = Core::ClassManager::getDynamicClassMetadata(pointer).mClassDefinition.mName;
		}
		else
		{
			
			className = smPointersToDynamicClassName.at(ptrU64);
		}

        smPointersToDynamicClassName.erase(ptrU64);

        CHECK_MSG(smAllocationsMap.contains(className), "No prevoius allocation for class: " + className.get());
        smAllocationsMap[className].mCurrentAllocations -= 1;
#endif
	}

private:
#ifdef ENGINE_BUILD_DEBUG
	inline static std::unordered_map<Core::HashedString, AllocationInfo> smAllocationsMap;
    inline static std::unordered_map<u64, Core::HashedString> smPointersToDynamicClassName;
#endif
};

NS_END