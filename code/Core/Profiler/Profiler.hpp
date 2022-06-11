#ifndef PROFILER_HPP
#define PROFILER_HPP

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include "Core/Time/TimeUtils.hpp"

#define PROFILER_TIMEMARK_START() Profiler::getInstance().timeMarkStart(__PRETTY_FUNCTION__);
#define PROFILER_TIMEMARK_END() Profiler::getInstance().timeMarkEnd(__PRETTY_FUNCTION__);

#define PROFILER_CPU() FunctionProfiler __functionProfiler(__PRETTY_FUNCTION__, false);
#define PROFILER_GPU() FunctionProfiler __functionProfilerGPU(__PRETTY_FUNCTION__, true);

class FunctionProfiler
{
public:
    FunctionProfiler(const std::string& name, bool isGPU = false);
    ~FunctionProfiler();

private:
    std::string mScopeName;
    bool mIsGPU = false;
};

class Profiler : public ObjectBase, public Singleton<Profiler>
{
	GENERATE_METADATA(Profiler)

public:
    Profiler();
    ~Profiler() override;

    void init();
    void update(f32 deltaTimeMillis);
    void terminate();
    void timeMarkStart(const std::string& name);
    void timeMarkEnd(const std::string& name);

    void timeMarkGPUStart(const std::string& name);
    void timeMarkGPUEnd(const std::string& name);

private:
    void printResult(const std::string& name, f32 time)  const;
    void printResultAverage(const std::string& name, f32 time)  const;

private:
	std::map<std::string, f32> mTimeMap;
	std::map<std::string, TimeMark> mTimeMarkMap;

	std::map<std::string, f32> mTimeGPUMap;
    std::map<std::string, TimeMarkGPU> mTimeMarkGPUMap;

	f32 mTotalTimeMillis = 0;
	u32 mFrameCounter = 0;
};

#endif