#pragma once

#include "Core/StdCore.hpp"

// #ifdef BUILD_WITH_EASY_PROFILER
// #include "easy/profiler.h"
// #define PROFILER_BLOCK_CPU(str, ...) EASY_BLOCK(str, __VA_ARGS__);
// #define PROFILER_CPU() PROFILER_BLOCK_CPU(__PRETTY_FUNCTION__);
// #define PROFILER_END_BLOCK() EASY_END_BLOCK;
// #else
// #define PROFILER_BLOCK_CPU(str, ...) 
// #define PROFILER_CPU()
// #define PROFILER_GPU()
// #define PROFILER_END_BLOCK()
// #endif

/*
    #include "Tracy.hpp"
#define LUXE_PROFILE ZoneScoped
#define LUXE_PROFILE_FRAME(x) FrameMark
#define LUXE_PROFILE_SECTION(x) ZoneScopedN(x)
#define LUXE_PROFILE_TAG(y, x) ZoneText(x, strlen(x))
#define LUXE_PROFILE_LOG(text, size) TracyMessage(text, size)
#define LUXE_PROFILE_VALUE(text, value) TracyPlot(text, value)

#include "TracyC.h"
#define LUXE_PROFILE_ALLOC(p, size) TracyCAllocS(p, size, 12)
#define LUXE_PROFILE_FREE(p) TracyCFreeS(p, 12)

  #include "TracyOpenGL.hpp"
  #define LUXE_PROFILE_GPU(x) TracyGpuZone(x)

  SDL_GL_CreateContext(window);
TracyGpuContext;

SDL_GL_SwapWindow(window);
TracyGpuCollect;
*/
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"
#include "glew-2.2.0/include/GL/glew.h"
#include "tracy/TracyOpenGL.hpp"
#ifdef TRACY_ENABLE
#define PROFILER_BLOCK_CPU(varName) ZoneNamedN(__tracy_profiler##varName, #varName, true);
#define PROFILER_CPU() ZoneScopedN(__PRETTY_FUNCTION__);
#define PROFILER_END_BLOCK()
#else
#define PROFILER_BLOCK_CPU(varName)
#define PROFILER_CPU()
#define PROFILER_GPU()
#define PROFILER_END_BLOCK()
#endif

class Profiler
{
public:
    static void init();
    static void terminate();
};
