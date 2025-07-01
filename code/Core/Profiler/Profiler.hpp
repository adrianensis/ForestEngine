#pragma once

#include "Core/StdCore.hpp"

#ifdef ENGINE_ENABLE_PROFILER
#include "tracy/Tracy.hpp"
#include "tracy/TracyC.h"
#include "vulkan/vulkan.h"
#include "tracy/TracyVulkan.hpp"
#define PROFILER_CPU() ZoneScopedN(__PRETTY_FUNCTION__);
#define PROFILER_CPU_NAMED(varName) ZoneNamedN(__tracy_profiler_cpu_##varName, #varName, true);
#define PROFILER_GPU(context, commandBuffer, name) PROFILER_CPU() TracyVkZone(context, commandBuffer, name)
#define PROFILER_GPU_NAMED(varName, context, commandBuffer) PROFILER_CPU_NAMED(varName) TracyVkNamedZone(context, __tracy_profiler_gpu_##varName, commandBuffer, #varName, true)
#define PROFILER_GPU_CONTEXT(...) TracyVkContext(__VA_ARGS__)
#define PROFILER_GPU_CONTEXT_CALIBRATED(...) TracyVkContextCalibrated(__VA_ARGS__)
#define PROFILER_GPU_COLLECT(context, commandBuffer) TracyVkCollect(context, commandBuffer)
#define PROFILER_CALLSTACK_DEPTH 12
#define PROFILER_ALLOC(pointer, size) TracyCAllocS(pointer, size, PROFILER_CALLSTACK_DEPTH);
#define PROFILER_FREE(pointer) TracyCFreeS(pointer, PROFILER_CALLSTACK_DEPTH);
#else
#define PROFILER_CPU()
#define PROFILER_CPU_NAMED(varName)
#define PROFILER_GPU(context, commandBuffer, name)
#define PROFILER_GPU_NAMED(varName, context, commandBuffer)
#define PROFILER_GPU_CONTEXT(...)
#define PROFILER_GPU_CONTEXT_CALIBRATED(...)
#define PROFILER_GPU_COLLECT(context, commandBuffer)
#define PROFILER_CALLSTACK_DEPTH
#define PROFILER_ALLOC(pointer, size)
#define PROFILER_FREE(pointer)
#endif

NS_BEGIN(Core)
class Profiler
{
public:
    static void init();
    static void terminate();
};
NS_END