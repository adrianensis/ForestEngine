#pragma once

#include "Core/Core.hpp"

NS_BEGIN(Time)

enum class TimerDurationType
{
	NONE,
	TIME_AMOUNT,
	NEXT_FRAME
};

class Timer
{
public:
    void init(Core::f32 duration, TimerDurationType durationType, std::function<void()> callback);

public:
	Core::FunctorVoid mFunctor;
private:
	Core::f32 mDuration = 0.0f;
	Core::f32 mTimeCounter = 0.0f;
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