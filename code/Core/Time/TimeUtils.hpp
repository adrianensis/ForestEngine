#pragma once

#include "Core/Memory/Singleton.hpp"
#include <chrono>

NS_BEGIN(Time)
class TimeMark
{
public:
    void init();
    void start();
    void end();
    Core::f32 getElapsedTimeMillis();
    Core::f32 getElapsedTimeSeconds();
    Core::f32 getDeltaTimeMillis();
    Core::f32 getDeltaTimeSeconds();

private:
	Core::f32 mDeltaTimeMillis = 0.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> mLastTime;
	bool mIsStarted = false;
};

class TimeMarkGPU
{
public:

    void init();
    void start();
    void end();
    Core::f32 getDeltaTimeMillis();
    Core::f32 getDeltaTimeSeconds();

private:
	Core::u32 mQueryIDStart;
	Core::u32 mQueryIDEnd;

	Core::f32 mDeltaTimeMillis = 0.0f;
	Core::u32 mDeltaTimeChronoDuration;
	Core::u64 mStartTime;
	Core::u64 mLastTime;
	bool mIsStarted = false;
};

class Time: public Core::Singleton<Time>
{
public:
	void init() {mInternalTimeMark.init();}
	void startFrame() { mInternalTimeMark.start(); }
	void endFrame() { mInternalTimeMark.end(); }
	Core::f32 getElapsedTimeMillis() { return mInternalTimeMark.getElapsedTimeMillis(); }
	Core::f32 getElapsedTimeSeconds() { return mInternalTimeMark.getElapsedTimeSeconds(); }
	Core::f32 getDeltaTimeMillis() { return mInternalTimeMark.getDeltaTimeMillis(); }
	Core::f32 getDeltaTimeSeconds() { return mInternalTimeMark.getDeltaTimeSeconds(); }

private:
	TimeMark mInternalTimeMark;
};
NS_END