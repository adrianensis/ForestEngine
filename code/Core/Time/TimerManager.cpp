#include "Core/Time/TimerManager.hpp"
#include "Core/Profiler/Profiler.hpp"
 
NS_BEGIN(Time)
void TimerManager::endTimer(Timer * timer)
{
	mTimers.remove(timer);
	Core::Memory::deleteObject(timer);
}

void TimerManager::init()
{

}

void TimerManager::update(Core::f32 dt)
{
	PROFILER_CPU()
	if (!mTimers.empty())
	{
		Core::f32 deltaTime = dt/1000.f; // seconds
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

TimerHandle TimerManager::setTimer(Core::f32 duration, TimerDurationType durationType, std::function<void()> callback)
{
	Timer *timer = Core::Memory::newObject<Timer>();
	timer->init(duration, durationType, callback);

	TimerHandle timerHandle;
	timerHandle.init(timer);

	mTimers.push_back(timer);

	return timerHandle;
}

void TimerManager::cancelTimer(const TimerHandle& timerHandle)
{
	endTimer(timerHandle.mTimerReference);
}

void TimerManager::terminate()
{
	mTimers.clear();
}
NS_END