#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Metadata.hpp"
#include "Core/Assert/Assert.hpp"

class Memory
{
public:
	inline static std::map<std::string, i32> mAllocationsCounter;

    static void init();

    static void terminate();
	
	template <class T, typename ... Args>
	static T *newObject(Args&&... args)
	{
		T *object = new T(args...);

		std::string className;

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

		return object;
	}

	template <class T>
	static void deleteObject(T * pointer)
	{
		ASSERT_MSG(pointer != nullptr, "pointer is nullptr");

		std::string className;
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

		delete pointer;
	}
};
