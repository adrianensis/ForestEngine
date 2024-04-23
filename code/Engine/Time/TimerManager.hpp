#pragma once

#include "Engine/Systems/System.hpp"
#include "Engine/Time/TimeUtils.hpp"

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
	FunctorVoid mFunctor;
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

class TimerManager: public System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    TimerHandle setTimer(f32 duration, TimerDurationType durationType, std::function<void()> callback);
    void cancelTimer(const TimerHandle& timerHandle);

private:
    void endTimer(Timer * timer);

private:
	std::list<Timer *> mTimers;
};
REGISTER_CLASS(TimerManager);
