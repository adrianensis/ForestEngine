#pragma once

#include "Core/Time/Timer.hpp"

NS_BEGIN(Time)
class TimerManager
{
public:
    void init();
    void terminate();
    void update(Core::f32 dt);
    TimerHandle setTimer(Core::f32 duration, TimerDurationType durationType, std::function<void()> callback);
    void cancelTimer(const TimerHandle& timerHandle);

private:
    void endTimer(Timer * timer);

private:
	std::list<Timer *> mTimers;
};

NS_END