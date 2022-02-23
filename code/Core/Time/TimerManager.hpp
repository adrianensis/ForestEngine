#pragma once

#include "Core/Time/TimeUtils.hpp"
#include "Core/Functor.hpp"

enum class TimerDurationType
{
	NONE,
	TIME_AMOUNT,
	NEXT_FRAME
};

class Timer: public ObjectBase
{
    GENERATE_METADATA(Timer)
private:
	f32 mDuration = 0.0f; GET(Duration)
	f32 mTimeCounter = 0.0f; GET_SET(TimeCounter)
	TimerDurationType mDurationType; GET(DurationType)
public:
	FunctorVoid mFunctor;

public:
	void init(f32 duration, TimerDurationType durationType, std::function<void()> callback);
};

class TimerHandle: public ObjectBase
{
    GENERATE_METADATA(TimerHandle)
	friend class TimerManager;

	PRI Timer* mTimerReference = nullptr;

public:
	void init(Timer * timerReference)
	{
		mTimerReference = timerReference;
	}

	COPY(TimerHandle)
	{
		DO_COPY(mTimerReference)
	}
};

class TimerManager: public ObjectBase, public Singleton<TimerManager>
{
	GENERATE_METADATA(TimerManager)
	PRI std::list<Timer *> mTimers;

	void endTimer(Timer * timer);

public:
	~TimerManager() override;
	void init();
	TimerHandle setTimer(f32 duration, TimerDurationType durationType, std::function<void()> callback);
	void cancelTimer(const TimerHandle& timerHandle);
	void update();
	void terminate();
};