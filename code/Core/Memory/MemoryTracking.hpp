#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <typeinfo>
#include "Core/Assert/Assert.hpp"
#include "Core/Metadata/ClassManager.hpp"

class MemoryTracking
{
private:
#ifdef ENGINE_DEBUG
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
	
	template <class T>
	static void registerNewObject(T* pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");

#ifdef ENGINE_DEBUG
		std::string_view className;

		if (ClassManager::getClassMetadataNoAssert<T>().mClassDefinition.mId > 0)
		{
			className = ClassManager::getClassMetadata<T>().mClassDefinition.mName;
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
	static void unregisterDeletedObject(T* pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");

#ifdef ENGINE_DEBUG
		std::string_view className;
		if (ClassManager::getDynamicClassMetadata<T>(pointer).mClassDefinition.mId > 0)
		{
			className = ClassManager::getDynamicClassMetadata<T>(pointer).mClassDefinition.mName;
		}
		else
		{
			className = smPointersToDynamicClassName.at(reinterpret_cast<u64>(pointer));
		}

        smPointersToDynamicClassName.erase(reinterpret_cast<u64>(pointer));

        CHECK_MSG(smAllocationsMap.contains(className), "No prevoius allocation for class: " + std::string(className));
        smAllocationsMap[className].mCurrentAllocations -= 1;
#endif
	}

private:
#ifdef ENGINE_DEBUG
	inline static std::unordered_map<std::string_view, AllocationInfo> smAllocationsMap;
    inline static std::unordered_map<u64, std::string_view> smPointersToDynamicClassName;
#endif
};
