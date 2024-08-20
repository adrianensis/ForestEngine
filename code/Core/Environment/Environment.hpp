#pragma once

#if defined(_WIN64)
	#define ENGINE_PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define ENGINE_PLATFORM_MACOS
/* check for __android__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined
 */
#elif defined(__ANDROID__)
    #define ENGINE_PLATFORM_ANDROID
#elif defined(__linux__)
    #define ENGINE_PLATFORM_LINUX
#else
	#error "Unsupported platform"
#endif

#if defined(_MSC_VER)
    #define ENGINE_COMPILER_MSVC
#elif defined(__clang__)
    #define ENGINE_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define ENGINE_COMPILER_GCC
#else
    #error "Unknown compiler"
#endif

class Environment
{
public:

    enum class Platform
    {
        NONE,
        WINDOWS,
        MACOS,
        LINUX,
    };

    enum class Compiler
    {
        NONE,
        MSVC,
        CLANG,
        GCC,
    };

    enum class Build
    {
        NONE,
        DEBUG,
        RELEASE,
    };

    inline static const Platform mPlatform =
#if defined(ENGINE_PLATFORM_WINDOWS)
        Platform::WINDOWS;
#elif defined(ENGINE_PLATFORM_MACOS)
        Platform::MACOS;
#elif defined(ENGINE_PLATFORM_LINUX)
        Platform::LINUX;
#else
        Platform::NONE;
#endif

    inline static const Compiler mCompiler =
#if defined(ENGINE_COMPILER_MSVC)
        Compiler::MSVC;
#elif defined(ENGINE_COMPILER_CLANG)
        Compiler::CLANG;
#elif defined(ENGINE_COMPILER_GCC)
        Compiler::GCC;
#else
        Compiler::NONE;
#endif

    inline static const Build mBuild =
#if defined(ENGINE_BUILD_RELEASE)
        Build::RELEASE;
#elif defined(ENGINE_BUILD_DEBUG)
        Build::DEBUG;
#else
        Build::NONE;
#endif
};