#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Assert/Assert.hpp"
#include "Core/Metadata.hpp"

class Memory
{
public:
	inline static std::unordered_map<std::string_view, i32> mAllocationsCounter;

    static void init();

    static void terminate();
	
	template <class T, typename ... Args>
	static T *newObject(Args&&... args)
	{
		T *object = new T(args...);
#if ENABLE_PROFILER
		std::string_view className;

		if constexpr (std::is_base_of<ObjectMeta, T>::value)
		{
			className = object->getClassName();
		}
		else
		{
			className = typeid(T).name();
		}

		if (MAP_CONTAINS(mAllocationsCounter, className))
		{
			MAP_INSERT(mAllocationsCounter, className, mAllocationsCounter[className] + 1);
		}
		else
		{
			MAP_INSERT(mAllocationsCounter, className, 1);
		}
#endif
		return object;
	}

	template <class T>
	static void deleteObject(T * pointer)
	{
		ASSERT_MSG(pointer != nullptr, "pointer is nullptr");

#if ENABLE_PROFILER
		std::string_view className;
		if constexpr (std::is_base_of<ObjectMeta, T>::value)
		{
			className = pointer->getClassName();
		}
		else
		{
			className = typeid(T).name();
		}

		if (MAP_CONTAINS(mAllocationsCounter, className))
		{
			MAP_INSERT(mAllocationsCounter, className, mAllocationsCounter[className] - 1);
		}
#endif
		delete pointer;
	}
};
