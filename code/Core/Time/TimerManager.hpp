#pragma once

#include "Core/System/System.hpp"
#include "Core/Time/Timer.hpp"

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
