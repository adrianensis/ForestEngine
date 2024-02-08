#pragma once

#include "Core/StdPrimitiveTypes.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Hash.hpp"

// Metadata for base object
class IMemoryTrackedObject
{
public:
	IMemoryTrackedObject() = default;
	virtual const std::string_view& getMemoryTrackName() const = 0;
};

class Memory
{
private:
#ifdef ENGINE_DEBUG
	class AllocationInfo
    {
    public:
        AllocationInfo() = default;
        u32 mCurrentAllocations = 0;
        u32 mMaxAllocations = 0;
    };
#endif

public:
    static void init();

    static void terminate();
	
	template <class T, typename ... Args>
	static T *newObject(Args&&... args)
	{
		T *object = new T(args...);
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

		if (!smAllocationsCounter.contains(className))
        {
			smAllocationsCounter.insert_or_assign(className, AllocationInfo());
		}

        smAllocationsCounter[className].mCurrentAllocations += 1;
        smAllocationsCounter[className].mMaxAllocations = std::max(smAllocationsCounter[className].mCurrentAllocations, smAllocationsCounter[className].mMaxAllocations);
#endif
		return object;
	}

	template <class T>
	static void deleteObject(T * pointer)
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

        CHECK_MSG(smAllocationsCounter.contains(className), "No prevoius allocation for class: " + std::string(className));
        smAllocationsCounter[className].mCurrentAllocations -= 1;
#endif
		delete pointer;
	}

private:
#ifdef ENGINE_DEBUG
	inline static std::unordered_map<std::string_view, AllocationInfo> smAllocationsCounter;
#endif
};
