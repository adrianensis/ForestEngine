#include "Core/Profiler/Profiler.hpp"
#include "Core/Log/Log.hpp"

Profiler::Profiler()
{

}
Profiler::~Profiler() 
{

}

void Profiler::init()
{
#ifdef BUILD_WITH_EASY_PROFILER
	EASY_PROFILER_ENABLE;
    profiler::startListen();
#endif
}

void Profiler::terminate()
{
#ifdef BUILD_WITH_EASY_PROFILER
	// WARNING: It generates GBs of data!
	//u32 result = profiler::dumpBlocksToFile("test_profile.prof");
	profiler::stopListen();
#endif
}