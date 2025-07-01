#pragma once

#include "Core/System/System.hpp"
#include "Core/Time/Timer.hpp"

NS_BEGIN(Core)
class TimerManager: public Core::System
{
public:
    virtual void init() override;
    virtual void terminate() override;
    void update();
    TimerHandle setTimer(Core::f32 duration, TimerDurationType durationType, std::function<void()> callback);
    void cancelTimer(const TimerHandle& timerHandle);

private:
    void endTimer(Timer * timer);

private:
	std::list<Timer *> mTimers;
};
REGISTER_CLASS(TimerManager);

NS_END