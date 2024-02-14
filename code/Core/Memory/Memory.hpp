#pragma once

#include "Core/Assert/Assert.hpp"
#include "Core/Memory/MemoryTracking.hpp"

class Memory
{
public:
    static void init();

    static void terminate();
    static void check(bool condition, const std::string& conditionString);
	
	template <class T, typename ... Args>
	static T *newObject(Args&&... args)
	{
		T *object = new T(args...);
		CHECK_MSG(object != nullptr, "pointer is nullptr");
        MemoryTracking::registerNewObject(object);
		return object;
	}

	template <class T>
	static void deleteObject(T* pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");
        MemoryTracking::deleteObject(pointer);
		delete pointer;
	}
};
