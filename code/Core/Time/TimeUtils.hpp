#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Engine/EngineSystem.hpp"
#include <chrono>


class TimeMark: public ObjectBase
{
    GENERATE_METADATA(TimeMark)

public:
    void init();
    void start();
    void end();
    f32 getElapsedTimeMillis();
    f32 getElapsedTimeSeconds();
    f32 getDeltaTimeMillis();
    f32 getDeltaTimeSeconds();

private:
	f32 mDeltaTimeMillis = 0.0f;
	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> mLastTime;
	bool mIsStarted = false;

public:
	GET(IsStarted)
};

class TimeMarkGPU: public ObjectBase
{
    GENERATE_METADATA(TimeMarkGPU)

public:

    void init();
    void start();
    void end();
    f32 getDeltaTimeMillis();
    f32 getDeltaTimeSeconds();

private:
	u32 mQueryIDStart;
	u32 mQueryIDEnd;

	f32 mDeltaTimeMillis = 0.0f;
	u32 mDeltaTimeChronoDuration;
	u64 mStartTime;
	u64 mLastTime;
	bool mIsStarted = false;

public:
	GET(IsStarted)
};

class Time: public EngineSystem
{
	GENERATE_METADATA(Time)	

public:
	void init(){mInternalTimeMark.init();}
	void startFrame() { mInternalTimeMark.start(); }
	void endFrame() { mInternalTimeMark.end(); }
	f32 getElapsedTimeMillis() { return mInternalTimeMark.getElapsedTimeMillis(); }
	f32 getElapsedTimeSeconds() { return mInternalTimeMark.getElapsedTimeSeconds(); }
	f32 getDeltaTimeMillis() { return mInternalTimeMark.getDeltaTimeMillis(); }
	f32 getDeltaTimeSeconds() { return mInternalTimeMark.getDeltaTimeSeconds(); }

private:
	TimeMark mInternalTimeMark;
};

