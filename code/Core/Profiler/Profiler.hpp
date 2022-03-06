#pragma once

// CPP_IGNORE

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include "Core/Time/TimeUtils.hpp"

#define PROFILER_TIMEMARK_START() Profiler::getInstance().timeMarkStart(__PRETTY_FUNCTION__);
#define PROFILER_TIMEMARK_END() Profiler::getInstance().timeMarkEnd(__PRETTY_FUNCTION__);

class Profiler : public ObjectBase, public Singleton<Profiler>
{
	GENERATE_METADATA(Profiler)

private:
	std::map<std::string, f32> mTimeMap;
	std::map<std::string, TimeMark> mTimeMarkMap;

	f32 mTotalTimeMillis = 0;
	u32 mFrameCounter = 0;

	void printResult(const std::string& name, f32 time) const;
	void printResultAverage(const std::string& name, f32 time) const;

public:
	Profiler();
	~Profiler();

	void init();
	void update(f32 deltaTimeMillis);
	void terminate();

	void timeMarkStart(const std::string& name);
	void timeMarkEnd(const std::string& name);
};
