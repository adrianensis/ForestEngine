#pragma once

#include "Core/BasicTypes.hpp"
#include "Core/ObjectBase.hpp"
#include "Core/Singleton.hpp"
#include <chrono>

#ifdef CPP_INCLUDE
#include "Core/Time/TimeUtils.hpp"
#endif

class TimeMark: public ObjectBase
{
    GENERATE_METADATA(TimeMark)

public:

	COPY(TimeMark)
	{
		DO_COPY(mDeltaTimeMillis)
		DO_COPY(mDeltaTimeChronoDuration)
		DO_COPY(mStartTime)
		DO_COPY(mLastTime)
	}

	CPP void init()
	{
		mIsStarted = false;
		mDeltaTimeMillis = 0.0;
		mLastTime = std::chrono::high_resolution_clock::now();
	}

	CPP void start()
	{
		mIsStarted = true;
		mStartTime = std::chrono::high_resolution_clock::now();
	}

	CPP void end()
	{
		if (mIsStarted)
		{
			mIsStarted = false;
			mLastTime = std::chrono::high_resolution_clock::now();
			mDeltaTimeChronoDuration = std::chrono::duration_cast<std::chrono::milliseconds>(mLastTime - mStartTime);
			mDeltaTimeMillis = mDeltaTimeChronoDuration.count();
		}
	}

	CPP f32 getElapsedTimeMillis()
	{
		auto now = std::chrono::high_resolution_clock::now();
		return mIsStarted ? std::chrono::duration_cast<std::chrono::milliseconds>(now - mStartTime).count() : 0.0f;
	}

	CPP f32 getElapsedTimeSeconds()
	{
		return getElapsedTimeMillis() / 1000.0f;
	}

	CPP f32 getDeltaTimeMillis()
	{
		return mDeltaTimeMillis;
	}

	CPP f32 getDeltaTimeSeconds()
	{
		return getDeltaTimeMillis() / 1000.0f;
	}

private:
	f32 mDeltaTimeMillis = 0.0f;
	std::chrono::milliseconds mDeltaTimeChronoDuration;
	std::chrono::time_point<std::chrono::high_resolution_clock> mStartTime;
	std::chrono::time_point<std::chrono::high_resolution_clock> mLastTime;
	bool mIsStarted = false;

public:
	GET(IsStarted)
};

class Time: public ObjectBase, public Singleton<Time>
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
