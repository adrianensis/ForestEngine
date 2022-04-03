#pragma once

#include "Core/Time/TimeUtils.hpp"
#include "Core/Functor.hpp"

#ifdef CPP_INCLUDE
#include "Core/Time/TimerManager.hpp"
#include "Core/Profiler/Profiler.hpp"
#include "Core/Log/Log.hpp"
#endif

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
	f32 mDuration = 0.0f;
	f32 mTimeCounter = 0.0f;
	TimerDurationType mDurationType;
public:
	FunctorVoid mFunctor;

	GET(Duration)
	GET_SET(TimeCounter)
	GET(DurationType)

public:
	CPP void init(f32 duration, TimerDurationType durationType, std::function<void()> callback)
	{
		mDurationType = durationType;
		mDuration = duration;
		mFunctor.setCallback(callback);
	}
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
	
private:
	std::list<Timer *> mTimers;

	CPP void endTimer(Timer * timer)
	{
		mTimers.remove(timer);
		DELETE(timer);
	}

public:
	~TimerManager() override = default;
	
	CPP void init() {}

	CPP void update()
	{
		PROFILER_TIMEMARK_START()
		if (!mTimers.empty())
		{
			f32 deltaTime = Time::getInstance().getDeltaTimeSeconds(); // seconds
			std::list<Timer *> timers(mTimers);

			for (auto itTimer = timers.begin(); itTimer != timers.end(); ++itTimer)
			{
				Timer *timer = *itTimer;

				timer->setTimeCounter(timer->getTimeCounter() + deltaTime);

				if (timer->getTimeCounter() >= timer->getDuration())
				{
					timer->mFunctor.execute();
					endTimer(timer);
				}
			}
		}

		PROFILER_TIMEMARK_END()
	}

	CPP TimerHandle setTimer(f32 duration, TimerDurationType durationType, std::function<void()> callback)
	{
		Timer *timer = NEW(Timer);
		timer->init(duration, durationType, callback);

		TimerHandle timerHandler;
		timerHandler.init(timer);

		mTimers.push_back(timer);

		return timerHandler;
	}

	CPP void cancelTimer(const TimerHandle& timerHandle)
	{
		endTimer(timerHandle.mTimerReference);
	}

	CPP void terminate()
	{
		mTimers.clear();
	}
};