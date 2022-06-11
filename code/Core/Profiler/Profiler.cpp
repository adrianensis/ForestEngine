#include "Core/Profiler/Profiler.hpp"
#include "Core/Log/Log.hpp"

FunctionProfiler::FunctionProfiler(const std::string& name, bool isGPU /*= false*/)
{
	mScopeName = name;
	mIsGPU = isGPU;

	if(mIsGPU)
	{
		Profiler::getInstance().timeMarkGPUStart(mScopeName);
	}
	else
	{
		Profiler::getInstance().timeMarkStart(mScopeName);
	}
}

FunctionProfiler::~FunctionProfiler()
{
	if(mIsGPU)
	{
		Profiler::getInstance().timeMarkGPUEnd(mScopeName);
	}
	else
	{
		Profiler::getInstance().timeMarkEnd(mScopeName);
	}
}

Profiler::Profiler()
{

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

	if(mFrameCounter > 5)
	{
		mTotalTimeMillis += deltaTimeMillis;
	}
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

	FOR_MAP(it, mTimeGPUMap)
	{
		printResultAverage(it->first, it->second);
		ECHO("----------------------------")
	}

	ECHO("")

	mTimeMap.clear();

	mTimeMarkMap.clear();
}

void Profiler::timeMarkStart(const std::string& name)
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

void Profiler::timeMarkEnd(const std::string& name)
{
	if(MAP_CONTAINS(mTimeMap, name))
	{
		mTimeMarkMap.at(name).end();
		MAP_INSERT(mTimeMap, name, mTimeMap.at(name) + mTimeMarkMap.at(name).getDeltaTimeMillis());
	}
}

void Profiler::timeMarkGPUStart(const std::string& name)
{
	std::string gpuName = "(GPU) " + name;

	if(!MAP_CONTAINS(mTimeGPUMap, gpuName))
	{
		MAP_INSERT(mTimeGPUMap, gpuName, 0);

		TimeMarkGPU timeMark;
		MAP_INSERT(mTimeMarkGPUMap, gpuName, timeMark);
	}

	mTimeMarkGPUMap.at(gpuName).init();
	mTimeMarkGPUMap.at(gpuName).start();
}

void Profiler::timeMarkGPUEnd(const std::string& name)
{
	std::string gpuName = "(GPU) " + name;

	if(MAP_CONTAINS(mTimeGPUMap, gpuName))
	{
		mTimeMarkGPUMap.at(gpuName).end();
		MAP_INSERT(mTimeGPUMap, gpuName, mTimeGPUMap.at(gpuName) + mTimeMarkGPUMap.at(gpuName).getDeltaTimeMillis());
	}
}

void Profiler::printResult(const std::string& name, f32 time)  const
{
	ECHO(name + " : " + std::to_string(time) + "ms ")
}

void Profiler::printResultAverage(const std::string& name, f32 time)  const
{
	printResult(std::string("(Avg) ") + name, time / mFrameCounter);
}
