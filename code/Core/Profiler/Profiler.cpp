#include "Core/Profiler/Profiler.hpp"
#include "Core/Log/Log.hpp"


Profiler::Profiler() {

}
Profiler::~Profiler() 
{
	mTimeMap.clear();
	mTimeMarkMap.clear();
}

void Profiler::init() {

}
void Profiler::update(f32 deltaTimeMillis)
{
	mFrameCounter++;

	mTotalTimeMillis += deltaTimeMillis;
}

void Profiler::terminate()
{
	ECHO("")
	ECHO("PROFILER RESULTS")
	printResult("Total Time", mTotalTimeMillis);
	printResultAverage("Frame Time", mTotalTimeMillis);
	FOR_MAP(it, mTimeMap)
	{
		printResultAverage(it->first, it->second);
		ECHO("----------------------------")
	}

	ECHO("")

	mTimeMap.clear();

	mTimeMarkMap.clear();
}

void Profiler::timeMarkStart(const String& name)
{
	if(!MAP_CONTAINS(mTimeMap, name))
	{
		MAP_INSERT(mTimeMap, name, 0);

		TimeMark timeMark;
		MAP_INSERT(mTimeMarkMap, name, timeMark);
	}

	mTimeMarkMap.at(name).init();
	mTimeMarkMap.at(name).start();
}

void Profiler::timeMarkEnd(const String& name)
{
	if(MAP_CONTAINS(mTimeMap, name))
	{
		mTimeMarkMap.at(name).end();
		MAP_INSERT(mTimeMap, name, mTimeMap.at(name) + mTimeMarkMap.at(name).getDeltaTimeMillis());
	}
}

void Profiler::printResult(const String& name, f32 time)  const
{
	ECHO(name + " : " + std::to_string(time) + "ms ")
}

void Profiler::printResultAverage(const String& name, f32 time)  const
{
	printResult(String("(Avg) ") + name, time / mFrameCounter);
}
