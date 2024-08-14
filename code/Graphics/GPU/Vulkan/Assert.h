#pragma once

#include "Environment.h"
#include "Log.h"

#ifdef VD_BUILD_DEBUG
    #define VD_ENABLE_BREAK
    #define VD_ENABLE_ASSERT
#endif

#ifdef VD_ENABLE_BREAK
    #ifdef VD_PLATFORM_WINDOWS
        #define VD_BREAK() __debugbreak()
    #elif __has_builtin(__builtin_debugtrap)
        #define VD_BREAK() __builtin_debugtrap()
    #else
        #include <csignal>
        #if defined(SIGTRAP)
            #define VD_BREAK() std::raise(SIGTRAP)
        #else
            #define VD_BREAK() std::raise(SIGABRT)
        #endif
    #endif
#else
    #define VD_BREAK()
#endif

#ifdef VD_ENABLE_ASSERT
    #define VD_ASSERT(expression) \
        if (expression) \
        {} \
        else \
        { \
            VD_LOG_ERROR("Could not assert [{0}]", #expression); \
            VD_BREAK(); \
        }
#else
    #define VD_ASSERT(expression)
#endif