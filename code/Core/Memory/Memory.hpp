#pragma once

#include "Core/Assert/Assert.hpp"
#include "Core/Memory/MemoryTracking.hpp"
#include "Core/Metadata/ClassManager.hpp"
#include "Core/Profiler/Profiler.hpp"

NS_BEGIN(Core)
class Memory
{
public:
    static void init();

    static void terminate();
    static void check(bool condition, const std::string& conditionString);
	
	template <class T, typename ... Args>
	static T *newObject(Args&&... args)
	{
        PROFILER_CPU()
		T *object = new T(args...);
        PROFILER_ALLOC(object, sizeof(T))
		CHECK_MSG(object != nullptr, "pointer is nullptr");
        registerPointer(object);
		return object;
	}

    template <class T>
    static void registerPointer(const T* object)
	{
        PROFILER_CPU()
		CHECK_MSG(object != nullptr, "pointer is nullptr");
        if (Core::ClassManager::getClassMetadataNoAssert<T>().mClassDefinition.getId() > 0)
		{
            Core::ClassManager::registerDynamicClass<T>(object);
		}
        MemoryTracking::registerNewObject(object);
	}

	template <class T>
	static void deleteObject(T* pointer)
	{
        PROFILER_CPU()
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");
        unregisterPointer(pointer);
        PROFILER_FREE(pointer)
        delete pointer;
	}

    static void unregisterPointer(const void* pointer)
	{
        PROFILER_CPU()
		CHECK_MSG(pointer != nullptr, "pointer is nullptr");
        if (Core::ClassManager::getDynamicClassMetadata(pointer).mClassDefinition.getId() > 0)
		{
		    Core::ClassManager::unregisterDynamicClass(pointer);
        }
        MemoryTracking::unregisterDeletedObject(pointer);
	}
};

NS_END