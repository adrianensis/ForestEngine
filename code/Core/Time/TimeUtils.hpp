#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include <chrono>

class TimeMark: public ObjectBase
{
    GENERATE_METADATA(TimeMark)
	PRI f32 mDeltaTimeMillis = 0.0f;
	PRI std::chrono::milliseconds mDeltaTimeChronoDuration;
	PRI std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
	PRI std::chrono::time_point<std::chrono::high_resolution_clock> mLastTime;
	PRI bool mIsStarted = false; GET(IsStarted)

public:
	void init();
	void start();
	void end();
	f32 getElapsedTimeMillis();
	f32 getElapsedTimeSeconds();
	f32 getDeltaTimeMillis();
	f32 getDeltaTimeSeconds();

	COPY(TimeMark)
	{
		DO_COPY(mDeltaTimeMillis)
		DO_COPY(mDeltaTimeChronoDuration)
		DO_COPY(mStartTime)
		DO_COPY(mLastTime)
	}
};

class Time: public ObjectBase, public Singleton<Time>
{
	GENERATE_METADATA(Time)	
	PRI TimeMark mInternalTimeMark;

public:

	void init(){mInternalTimeMark.init();}
	void startFrame() { mInternalTimeMark.start(); }
	void endFrame() { mInternalTimeMark.end(); }
	f32 getElapsedTimeMillis() { return mInternalTimeMark.getElapsedTimeMillis(); }
	f32 getElapsedTimeSeconds() { return mInternalTimeMark.getElapsedTimeSeconds(); }
	f32 getDeltaTimeMillis() { return mInternalTimeMark.getDeltaTimeMillis(); }
	f32 getDeltaTimeSeconds() { return mInternalTimeMark.getDeltaTimeSeconds(); }
};
