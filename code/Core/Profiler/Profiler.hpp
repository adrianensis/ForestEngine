#pragma once

#include "Core/StdCore.hpp"
#include "Core/Object/ObjectBase.hpp"
#include "Core/Systems/System.hpp"

#ifdef BUILD_WITH_EASY_PROFILER
#include "easy/profiler.h"

// #define PROFILER_CPU() EASY_FUNCTION(profiler::colors::Magenta); EASY_BLOCK("ssss");
#define PROFILER_BLOCK_CPU(str, ...) EASY_BLOCK(str, __VA_ARGS__);
#define PROFILER_CPU() PROFILER_BLOCK_CPU(__PRETTY_FUNCTION__);
#define PROFILER_END_BLOCK() EASY_END_BLOCK;

//ProfilerBlock __ProfilerBlock(__PRETTY_FUNCTION__, false);
#define PROFILER_GPU() //ProfilerBlock __ProfilerBlockGPU(__PRETTY_FUNCTION__, true);
#else
#define PROFILER_BLOCK_CPU(str, ...) 
#define PROFILER_CPU()
#define PROFILER_GPU()
#define PROFILER_END_BLOCK()
#endif

class Profiler : public System
{
	GENERATE_METADATA(Profiler)

public:
    Profiler();
    ~Profiler() override;

    void init();
    void terminate();
};
