#include "Core/Time/TimerManager.hpp"
#include "Core/Profiler/Profiler.hpp"
#include "Core/Log/Log.hpp"


void Timer::init(f32 duration, TimerDurationType durationType, std::function<void()> callback)
{
	mDurationType = durationType;
	mDuration = duration;
	mFunctor.setCallback(callback);
}

void TimerManager::endTimer(Timer * timer)
{
	mTimers.remove(timer);
	Memory::deleteObject(timer);
}

void TimerManager::init() {

}
void TimerManager::update()
{
	PROFILER_CPU()
	if (!mTimers.empty())
	{
		f32 deltaTime = GET_SYSTEM(Time).getDeltaTimeSeconds(); // seconds
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

}

TimerHandle TimerManager::setTimer(f32 duration, TimerDurationType durationType, std::function<void()> callback)
{
	Timer *timer = Memory::newObject<Timer>();
	timer->init(duration, durationType, callback);

	TimerHandle timerHandler;
	timerHandler.init(timer);

	mTimers.push_back(timer);

	return timerHandler;
}

void TimerManager::cancelTimer(const TimerHandle& timerHandle)
{
	endTimer(timerHandle.mTimerReference);
}

void TimerManager::terminate()
{
	mTimers.clear();
}
