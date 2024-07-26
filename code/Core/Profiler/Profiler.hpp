#pragma once

#include "Core/StdCore.hpp"

#ifdef ENGINE_ENABLE_PROFILER
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"
#include "glew-2.2.0/include/GL/glew.h"
#include "tracy/TracyOpenGL.hpp"
#define PROFILER_BLOCK_CPU(varName) ZoneNamedN(__tracy_profiler##varName, #varName, true);
#define PROFILER_CPU() ZoneScopedN(__PRETTY_FUNCTION__);
#define PROFILER_GPU() TracyGpuZone(__PRETTY_FUNCTION__)
#define PROFILER_END_BLOCK()
#define PROFILER_GPU_CONTEXT() TracyGpuContext;
#define PROFILER_GPU_COLLECT() TracyGpuCollect;
#define PROFILER_CALLSTACK_DEPTH 12
#define PROFILER_ALLOC(pointer, size) TracyCAllocS(pointer, size, PROFILER_CALLSTACK_DEPTH);
#define PROFILER_FREE(pointer) TracyCFreeS(pointer, PROFILER_CALLSTACK_DEPTH);
#else
#define PROFILER_BLOCK_CPU(varName)
#define PROFILER_CPU()
#define PROFILER_GPU()
#define PROFILER_END_BLOCK()
#define PROFILER_GPU_CONTEXT()
#define PROFILER_GPU_COLLECT()
#define PROFILER_ALLOC(pointer, size)
#define PROFILER_FREE(pointer)
#endif

class Profiler
{
public:
    static void init();
    static void terminate();
};
