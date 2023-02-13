#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/EngineSystem.hpp"
#include "Core/Time/TimeUtils.hpp"
#include "easy/profiler.h"

#ifdef BUILD_WITH_EASY_PROFILER
#define PROFILER_CPU() EASY_FUNCTION(profiler::colors::Magenta); //ProfilerBlock __ProfilerBlock(__PRETTY_FUNCTION__, false);
#define PROFILER_GPU() //ProfilerBlock __ProfilerBlockGPU(__PRETTY_FUNCTION__, true);
#else
#define PROFILER_CPU()
#define PROFILER_GPU()
#endif

class Profiler : public EngineSystem
{
	GENERATE_METADATA(Profiler)

public:
    Profiler();
    ~Profiler() override;

    void init();
    void terminate();
};
