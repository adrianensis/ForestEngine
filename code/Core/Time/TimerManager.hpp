#ifndef TIMERMANAGER_HPP
#define TIMERMANAGER_HPP

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

class TimerHandle: public ObjectBase
{
    GENERATE_METADATA(TimerHandle)
	friend class TimerManager;

private:
	Timer* mTimerReference = nullptr;

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
	
public:
	~TimerManager() override = default;
	
    void init();
    void update();
    TimerHandle setTimer(f32 duration, TimerDurationType durationType, std::function<void()> callback);
    void cancelTimer(const TimerHandle& timerHandle);
    void terminate();

private:
    void endTimer(Timer * timer);

private:
	std::list<Timer *> mTimers;
};

#endif