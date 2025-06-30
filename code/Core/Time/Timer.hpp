#pragma once

#include "Core/Std.hpp"

NS_BEGIN(Core)

enum class TimerDurationType
{
	NONE,
	TIME_AMOUNT,
	NEXT_FRAME
};

class Timer
{
public:
    void init(f32 duration, TimerDurationType durationType, std::function<void()> callback);

public:
	Core::FunctorVoid mFunctor;
private:
	f32 mDuration = 0.0f;
	f32 mTimeCounter = 0.0f;
	TimerDurationType mDurationType;

public:
	GET(Duration)
	GET_SET(TimeCounter)
	GET(DurationType)
};

class TimerHandle
{
	friend class TimerManager;

private:
	Timer* mTimerReference = nullptr;

public:
	void init(Timer * timerReference)
	{
		mTimerReference = timerReference;
	}
};
NS_END