#pragma once

#include <string>
#include <string_view>
#include <unordered_map>
#include <typeinfo>
#include "Core/Assert/Assert.hpp"

// Metadata for base object
class IMemoryTrackedObject
{
public:
	IMemoryTrackedObject() = default;
	virtual const std::string_view& getMemoryTrackName() const = 0;
};

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
	static void registerNewObject(T* object)
	{
		CHECK_MSG(object != nullptr, "pointer is nullptr");

#ifdef ENGINE_DEBUG
		std::string_view className;

		if constexpr (std::is_base_of<IMemoryTrackedObject, T>::value)
		{
			className = object->getMemoryTrackName();
		}
		else
		{
			className = typeid(T).name();
		}

		if (!smAllocationsMap.contains(className))
        {
			smAllocationsMap.insert_or_assign(className, AllocationInfo());
		}

        smAllocationsMap[className].mCurrentAllocations += 1;
        smAllocationsMap[className].mMaxAllocations = std::max(smAllocationsMap[className].mCurrentAllocations, smAllocationsMap[className].mMaxAllocations);
#endif
	}

	template <class T>
	static void deleteObject(T* pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");

#ifdef ENGINE_DEBUG
		std::string_view className;
		if constexpr (std::is_base_of<IMemoryTrackedObject, T>::value)
		{
			className = pointer->getMemoryTrackName();
		}
		else
		{
			className = typeid(T).name();
		}

        CHECK_MSG(smAllocationsMap.contains(className), "No prevoius allocation for class: " + std::string(className));
        smAllocationsMap[className].mCurrentAllocations -= 1;
#endif
	}

private:
#ifdef ENGINE_DEBUG
	inline static std::unordered_map<std::string_view, AllocationInfo> smAllocationsMap;
#endif
};
