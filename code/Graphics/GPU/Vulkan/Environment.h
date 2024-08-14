#pragma once

#if defined(_WIN64)
	#define VD_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
    #define VD_PLATFORM_MACOS
#else
	//#error "Unsupported platform"
#endif

#if defined(_MSC_VER)
    #define VD_COMPILER_MSVC
#elif defined(__clang__)
    #define VD_COMPILER_CLANG
#elif defined(__GNUC__) || defined(__GNUG__)
    #define VD_COMPILER_GCC
#else
    #error "Unknown compiler"
#endif

#if defined(NDEBUG)
	#define VD_BUILD_RELEASE
#else
	#define VD_BUILD_DEBUG
#endif

namespace GPUAPI {

    enum class Platform {
        None = 0,
        Windows = 1,
        MacOS = 2,
        Linux = 3,
    };

    enum class Compiler {
        None = 0,
        MSVC = 1,
        Clang = 2,
        GCC = 3,
    };

    enum class Build {
        None = 0,
        Debug = 1,
        Release = 2,
    };

    class Environment {
    public:
        static Platform getPlatform() {
#if defined(VD_PLATFORM_WINDOWS)
            return Platform::Windows;
#elif defined(VD_PLATFORM_MACOS)
            return Platform::MacOS;
#elif defined(VD_PLATFORM_LINUX)
            return Platform::Linux;
#else
        return Platform::None;
#endif
        }

        static bool isWindows() {
            return getPlatform() == Platform::Windows;
        }

        static bool isMacOS() {
            return getPlatform() == Platform::MacOS;
        }

        static bool isLinux() {
            return getPlatform() == Platform::Linux;
        }

        static Compiler getCompiler() {
#if defined(VD_COMPILER_MSVC)
            return Compiler::MSVC;
#elif defined(VD_COMPILER_CLANG)
            return Compiler::Clang;
#elif defined(VD_COMPILER_GCC)
            return Compiler::GCC;
#endif
        }

        static bool isMSVC() {
            return getCompiler() == Compiler::MSVC;
        }

        static bool isClang() {
            return getCompiler() == Compiler::Clang;
        }

        static bool isGCC() {
            return getCompiler() == Compiler::GCC;
        }

        static Build getBuild() {
#if defined(VD_BUILD_RELEASE)
            return Build::Release;
#elif defined(VD_BUILD_DEBUG)
            return Build::Debug;
#endif
        }

        static bool isRelease() {
            return getBuild() == Build::Release;
        }

        static bool isDebug() {
            return getBuild() == Build::Debug;
        }
    };
}