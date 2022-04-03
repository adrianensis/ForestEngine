#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/Metadata.hpp"
#include "Core/Assert/Assert.hpp"

#ifdef CPP_INCLUDE
#include "Core/Memory.hpp"
#include "Core/Log/Log.hpp"
#endif

class Memory
{
public:
	inline static std::map<std::string, i32> mAllocationsCounter;

	CPP static void init()
	{
	}

	CPP static void terminate()
	{
		ECHO("-------- MEM SUMMARY --------")
		FOR_MAP(it, Memory::mAllocationsCounter)
		{
			std::cout << it->first << ": " << it->second << std::endl;
		}
		ECHO("-----------------------------")
	}
	template <class T>
	static T *newObject()
	{
		T *object = new T;

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