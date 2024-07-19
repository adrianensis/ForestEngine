#pragma once

#include "Core/Assert/Assert.hpp"
#include "Core/Memory/MemoryTracking.hpp"
#include "Core/Metadata/ClassManager.hpp"

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
        registerPointer(object);
		return object;
	}

    template <class T>
    static void registerPointer(const T* object)
	{
		CHECK_MSG(object != nullptr, "pointer is nullptr");
        if (ClassManager::getClassMetadataNoAssert<T>().mClassDefinition.getId() > 0)
		{
            ClassManager::registerDynamicClass<T>(object);
		}
        MemoryTracking::registerNewObject(object);
	}

	template <class T>
	static void deleteObject(T* pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");
        unregisterPointer(pointer);
        delete pointer;
	}

    static void unregisterPointer(const void* pointer)
	{
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");
        if (ClassManager::getDynamicClassMetadata(pointer).mClassDefinition.getId() > 0)
		{
		    ClassManager::unregisterDynamicClass(pointer);
        }
        MemoryTracking::unregisterDeletedObject(pointer);
	}
};
