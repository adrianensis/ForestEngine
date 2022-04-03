#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include "Core/Time/TimeUtils.hpp"

#ifdef CPP_INCLUDE
#include "Core/Profiler/Profiler.hpp"
#include "Core/Log/Log.hpp"
#endif

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

	CPP void printResult(const std::string& name, f32 time)  const
	{
		ECHO(name + " : " + std::to_string(time) + "ms ")
	}

	CPP void printResultAverage(const std::string& name, f32 time)  const
	{
		printResult(std::string("(Avg) ") + name, time / mFrameCounter);
	} 

public:
	CPP Profiler() {}
	CPP ~Profiler() override
	{
		mTimeMap.clear();
		mTimeMarkMap.clear();
	}

	CPP void init() {}

	CPP void update(f32 deltaTimeMillis)
	{
		mFrameCounter++;

		mTotalTimeMillis += deltaTimeMillis;
	}

	CPP void terminate()
	{
		ECHO("")
		ECHO("PROFILER RESULTS")
		printResult("Total Time", mTotalTimeMillis);
		printResultAverage("Frame Time", mTotalTimeMillis);
		FOR_MAP(it, mTimeMap)
		{
			//printResult(it->first, it->second);
			printResultAverage(it->first, it->second);
			ECHO("----------------------------")
		}

		ECHO("")

		mTimeMap.clear();

		mTimeMarkMap.clear();
	}

	CPP void timeMarkStart(const std::string& name)
	{
		// TODO: make it frame-time relative, maybe average?
		if(!MAP_CONTAINS(mTimeMap, name))
		{
			MAP_INSERT(mTimeMap, name, 0);

			TimeMark timeMark;
			MAP_INSERT(mTimeMarkMap, name, timeMark);
		}

		mTimeMarkMap.at(name).init();
		mTimeMarkMap.at(name).start();
	}

	CPP void timeMarkEnd(const std::string& name)
	{
		if(MAP_CONTAINS(mTimeMap, name))
		{
			mTimeMarkMap.at(name).end();
			MAP_INSERT(mTimeMap, name, mTimeMap.at(name) + mTimeMarkMap.at(name).getDeltaTimeMillis());
		}
	}
};
