#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Object/ObjectMetadata.hpp"

class Memory
{
private:
#ifdef DE_DEBUG
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
#ifdef DE_DEBUG
		std::string_view className;

		if constexpr (std::is_base_of<ObjectMeta, T>::value)
		{
			className = object->getClassName();
		}
		else
		{
			className = typeid(T).name();
		}

		if (!MAP_CONTAINS(mAllocationsCounter, className))
        {
			MAP_INSERT(mAllocationsCounter, className, AllocationInfo());
		}

        mAllocationsCounter[className].mCurrentAllocations += 1;
        mAllocationsCounter[className].mMaxAllocations = std::max(mAllocationsCounter[className].mCurrentAllocations, mAllocationsCounter[className].mMaxAllocations);
#endif
		return object;
	}

	template <class T>
	static void deleteObject(T * pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");

#ifdef DE_DEBUG
		std::string_view className;
		if constexpr (std::is_base_of<ObjectMeta, T>::value)
		{
			className = pointer->getClassName();
		}
		else
		{
			className = typeid(T).name();
		}

        CHECK_MSG(MAP_CONTAINS(mAllocationsCounter, className), "No prevoius allocation for class: " + std::string(className));
        mAllocationsCounter[className].mCurrentAllocations -= 1;
#endif
		delete pointer;
	}

private:
#ifdef DE_DEBUG
	inline static std::unordered_map<std::string_view, AllocationInfo> mAllocationsCounter;
#endif
};
